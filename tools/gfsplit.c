/*
 * Copyright Daniel Silverstone <dsilvers@digital-scurf.org> 2006-2011
 */

#include "config.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "libgfshare.h"

#define DEFAULT_SHARECOUNT 5
#define DEFAULT_THRESHOLD 3
#define BUFFER_SIZE 4096

#ifndef MIN
#define MIN(a,b) ((a)<(b))?(a):(b)
#endif



static char* progname;

void
usage(FILE* stream)
{
  fprintf( stream, "\
Usage: %s [-n threshold] [-m sharecount] inputfile [outputstem]\n\
  where sharecount is the number of shares to build.\n\
  where threshold is the number of shares needed to recombine.\n\
  where inputfile is the file to split.\n\
  where outputstem is the stem for the output files.\n\
\n\
The sharecount option defaults to %d.\n\
The threshold option defaults to %d.\n\
The outputstem option defaults to the inputfile.\n\
\n\
The program automatically adds \".NNN\" to the output stem for each share.\n\
", progname, DEFAULT_SHARECOUNT, DEFAULT_THRESHOLD );
}

static int
do_gfsplit( unsigned int sharecount, 
            unsigned int threshold,
            char *_inputfile,
            char *_outputstem )
{
  FILE *inputfile;
  unsigned char* sharenrs = malloc( sharecount );
  unsigned int i, result;
  FILE **outputfiles = malloc( sizeof(FILE*) * sharecount );
  char **outputfilenames = malloc( sizeof(char*) * sharecount );
  char* outputfilebuffer = malloc( strlen(_outputstem) + 5 );
  gfshare_ctx *G;
  
  if( sharenrs == NULL || outputfiles == NULL || outputfilenames == NULL || outputfilebuffer == NULL ) {
    perror( "malloc" );
    return 1;
  }
  
  inputfile = fopen( _inputfile, "rb" );
  if( inputfile == NULL ) {
    perror( _inputfile );
    return 1;
  }

  gfshare_generate_sharenrs(sharenrs, sharecount);

  for( i = 0; i < sharecount; ++i ) {
    sprintf( outputfilebuffer, "%s.%03d", _outputstem, sharenrs[i] );
    outputfiles[i] = fopen( outputfilebuffer, "wb" );
    if( outputfiles[i] == NULL ) {
      perror(outputfilebuffer);
      return 1;
    }
    outputfilenames[i] = strdup(outputfilebuffer);
  }
  /* All open, all ready and raring to go... */
  G = gfshare_ctx_init_enc( sharenrs, sharecount, 
                            threshold, MIN(BUFFER_SIZE, gfshare_file_getlen( inputfile )) );
  if( !G ) {
    perror("gfshare_ctx_init_enc");
    return 1;
  }

  result = gfshare_ctx_enc_stream(G, inputfile, outputfiles);
  gfshare_ctx_free( G );

  if( result != 0 )
    return result;

  fclose(inputfile);
  for( i = 0; i < sharecount; ++i ) {
    fclose(outputfiles[i]);
  }
  return 0;
}

#define OPTSTRING "n:m:hv"
int
main( int argc, char **argv )
{
  unsigned int sharecount = DEFAULT_SHARECOUNT;
  unsigned int threshold = DEFAULT_THRESHOLD;
  char *inputfile;
  char *outputstem;
  char *endptr;
  int optnr;
  
  progname = argv[0];
  srandom( time(NULL) );

  if (access("/dev/urandom", R_OK) == 0) {
  } else {
    fprintf(stderr, "\
%s: Cannot access /dev/urandom, so using rand() instead (not secure!)\n\
", progname);
    gfshare_fill_rand = gfshare_fill_rand_using_random;
  }
  
  while( (optnr = getopt(argc, argv, OPTSTRING)) != -1 ) {
    switch( optnr ) {
    case 'v':
      fprintf( stdout, "%s", "\
gfsplit (" PACKAGE_STRING ")\n\
Written by Daniel Silverstone.\n\
\n\
Copyright 2006-2011 Daniel Silverstone <dsilvers@digital-scurf.org>\n\
This is free software; see the source for copying conditions.  There is NO\n\
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\
" );
      return 0;
      break;
    case 'h':
      fprintf( stdout, "%s", "gfsplit (" PACKAGE_STRING ")\n");
      usage( stdout );
      return 0;
      break;
    case 'm':
      sharecount = strtoul( optarg, &endptr, 10 );
      if( *endptr != 0 || *optarg == 0 || 
          sharecount < 2 || sharecount > 255 ) {
        fprintf( stderr, "%s: Invalid argument to option -m\n", progname );
        usage( stderr );
        return 1;
      }
      break;
    case 'n':
      threshold = strtoul( optarg, &endptr, 10 );
      if( *endptr != 0 || *optarg == 0 || 
          threshold < 2 || threshold > sharecount) {
        fprintf( stderr, "%s: Invalid argument to option -n\n", progname );
        usage( stderr );
        return 1;
      }
      break;
    }
  }
  if( optind == argc || optind < (argc - 2) ) {
    fprintf( stderr, "%s: Bad argument count\n", progname );
    usage( stderr );
    return 1;
  }
  inputfile = argv[optind++];
  outputstem = (argc == optind)?inputfile:argv[optind++];
  return do_gfsplit( sharecount, threshold, inputfile, outputstem );
}
