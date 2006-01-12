/*
 * This file is Copyright Daniel Silverstone <dsilvers@digital-scurf.org> 2006
 */

#include "libgfshare.h"

#include <stdio.h>
#include <alloca.h>

#define MAXCOUNT 11

int
check_at_threshold(unsigned char secret,
                   unsigned int count,
                   unsigned int threshold,
                   unsigned char *sharenrs)
{
  unsigned char* shares = alloca(count);
  unsigned char* recombnrs = alloca(threshold);
  unsigned char* recombshn = alloca(threshold);
  unsigned char* recombshr = alloca(threshold);
  unsigned int i, p = 1;
  int ok = 1;
  
  gfshare_ll_bytewise_split(secret, threshold, count, shares, sharenrs);
  for( i = 0; i < threshold; ++i )
    recombnrs[i] = i;
  
  while( p >= 1 ) {
    for( i = 0; i < threshold; ++i ) {
      recombshr[i] = shares[recombnrs[i]];
      recombshn[i] = sharenrs[recombnrs[i]];
    }
    i = gfshare_ll_bytewise_combine(threshold, recombshr, recombshn);
    if( i != secret ) {
      fprintf(stdout, "\nBAD: Secret 0x%02x. Threshold %d. Count %d. Recombination 0x%02x\n", secret, threshold, count, i);
      for( i = 0; i < threshold; ++i ) 
        fprintf(stdout, "%02x ", recombshn[i]);
      fprintf(stdout,"\n");
      ok = 0;
    }
    /* Generate the next combination */
    if( recombnrs[threshold-1] == count - 1 )
      p -= 1;
    else
      p = threshold - 1;
    if( p >= 1 ) {
      for( i = threshold - 1; i >= p; --i )
        recombnrs[i] = recombnrs[p] + i - p + 1;
    }
  }
  
  
  return ok;
}

int
check_all_given(unsigned char secret,
                unsigned int count)
{
  unsigned char *sharenrs = alloca(count);
  unsigned int i, p = 1;
  
  int ok = 1;
  
  for( i = 0; i < count; ++i )
    sharenrs[i] = i;

  while( p >= 1 ) {
    /*
      fprintf(stdout, "CHOOSE:");
      for( i = 0; i < count; ++i )
      fprintf(stdout, " %02x", sharenrs[i]);
      fprintf(stdout, "\n");
    */
    for( i = 2; i <= count; ++i )
      ok &= check_at_threshold(secret, count, i, sharenrs);
    /* Generate the next combination */
    if( sharenrs[count-1] == MAXCOUNT - 1 )
      p -= 1;
    else
      p = count - 1;
    if( p >= 1 ) {
      for( i = count - 1; i >= p; --i )
        sharenrs[i] = sharenrs[p] + i - p + 1;
    }
  }
  
  return ok;
}

int 
main(int argc, char **argv)
{
  unsigned int secret, count;
  unsigned ok = 1;
  static char* throbber = "-/|\\";
  int throb = -1;
  
  fprintf(stdout, "test_gfshare_bytewise: ");
  
  for( secret = 0; secret < 256; ++secret ) {
    fprintf(stdout, "%c\b", throbber[++throb % 4]);
    fflush(stdout);
    for( count = 3; count < MAXCOUNT - 1; ++count ) {
      fflush(stdout);
      ok &= check_all_given(secret, count);
    }
  }
  
  fprintf(stdout,"OK\n");
  return !ok;
}
