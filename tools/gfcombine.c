/*
 * Copyright Daniel Silverstone <dsilvers@digital-scurf.org> 2006-2011
 */

#include "config.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "libgfshare.h"

#define BUFFER_SIZE 4096

#ifndef MIN
#define MIN(a,b) ((a)<(b))?(a):(b)
#endif

static char* progname;

void
usage(FILE* stream)
{
  fprintf( stream, "\
Usage: %s [-o outputfile] inputfile inputfile2...\n\
  where outputfile is the filename to write the combined result to.\n\
  where inputfile[2...] are the shares to recombine.\n\
\n\
If outputfile is not provided, it is automatically created by stripping the\n\
last four characters off the first input file name.\n\
\n\
Each input file must be the same length and the filenames must end in a\n\
number which will be taken to be the share number. I.E. \".NNN\".\n\
", progname );
}

static void
bad_filename( char* fname )
{
  fprintf( stderr, "%s: %s: bad filename\nInput files should be called <name>.NNN\n", progname, fname );
}

static void
zero_filename( char* fname )
{
  fprintf( stderr, "%s: %s: input files <name>.000 don't work, see README\n", progname, fname );
}

static int
check_filenames( char **filenames, int count )
{
  int i;
  if( count < 2 ) {
    fprintf(stderr, "%s: Insufficient input files. (Min of 2 for recombination)\n", progname);
    return 1;
  }
  for( i = 0; i < count; ++i ) {
    int nlen = strlen(filenames[i]);
    if( nlen < 5 ) {
      bad_filename(filenames[i]);
      return 1;
    }
    if( filenames[i][nlen-4] != '.' ||
        !isdigit(filenames[i][nlen-3]) ||
        !isdigit(filenames[i][nlen-2]) ||
        !isdigit(filenames[i][nlen-1]) ) {
      bad_filename(filenames[i]);
      return 1;
    }
    if( filenames[i][nlen-3] == '0' &&
        filenames[i][nlen-2] == '0' &&
        filenames[i][nlen-1] == '0') {
      zero_filename(filenames[i]);
      return 1;
    }
  }
  return 0;
}

static int
do_gfcombine( char *outputfilename, char **inputfilenames, int filecount )
{
  FILE *outfile;
  FILE **inputfiles = malloc( sizeof(FILE*) * filecount );
  unsigned char* sharenrs = malloc( filecount );
  int i, result;
  gfshare_ctx *G;
  unsigned int len1 = 0;
  
  if( inputfiles == NULL || sharenrs == NULL ) {
    perror( "malloc" );
    return 1;
  }
  
  if (strcmp(outputfilename, "-") == 0)
    outfile = fdopen(STDOUT_FILENO, "w");
  else 
    outfile = fopen( outputfilename, "wb" );

  if( outfile == NULL ) {
    perror((strcmp(outputfilename, "-") == 0) ? "standard out" : outputfilename);
    return 1;
  }
  for( i = 0; i < filecount; ++i ) {
    inputfiles[i] = fopen( inputfilenames[i], "rb" );
    if( inputfiles[i] == NULL ) {
      perror(inputfilenames[i]);
      return 1;
    }
    sharenrs[i] = strtoul( inputfilenames[i] + strlen(inputfilenames[i]) - 3, 
                           NULL, 10 );
    if( i == 0 ) len1 = gfshare_file_getlen(inputfiles[0]);
    else {
      if( len1 != gfshare_file_getlen(inputfiles[1]) ) {
        fprintf( stderr, "%s: File length mismatch between input files.\n", progname );
        return 1;
      }
    }
  }
  
  G = gfshare_ctx_init_dec( sharenrs, filecount, BUFFER_SIZE );
  result = gfshare_ctx_dec_stream( G, filecount, inputfiles, outfile );
  gfshare_ctx_free( G );
  if( result != 0 )
    return result;

  fclose(outfile);
  for( i = 0; i < filecount; ++i ) fclose(inputfiles[i]);
  return 0;
}

#define OPTSTRING "o:hv"
int
main( int argc, char **argv )
{
  int optnr;
  char *outputfile = NULL;
  
  progname = argv[0];
  
  while( (optnr = getopt(argc, argv, OPTSTRING)) != -1 ) {
    switch( optnr ) {
    case 'v':
      fprintf( stdout, "%s", "\
gfcombine (" PACKAGE_STRING ")\n\
Written by Daniel Silverstone.\n\
\n\
Copyright 2006-2011 Daniel Silverstone <dsilvers@digital-scurf.org>\n\
This is free software; see the source for copying conditions.  There is NO\n\
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\
" );
      return 0;
      break;
    case 'h':
      fprintf( stdout, "%s", "gfcombine (" PACKAGE_STRING ")\n");
      usage( stdout );
      return 0;
      break;
    case 'o':
      outputfile = optarg;
      break;
    }
  }
  
  if( check_filenames(argv+optind, argc-optind) ) return 1;
  
  if( outputfile == NULL ) {
    outputfile = strdup(argv[optind]);
    outputfile[strlen(outputfile)-4] = 0;
  }
  
  return do_gfcombine(outputfile, argv+optind, argc-optind);
}
