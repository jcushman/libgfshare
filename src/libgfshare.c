/*
 * This file is Copyright Daniel Silverstone <dsilvers@digital-scurf.org> 2006
 */

#include "config.h"
#include "libgfshare.h"
#include "libgfshare_tables.h"

#include <alloca.h>
#include <stdlib.h>

void 
bytewise_split(unsigned char secret,
               unsigned char threshold,
               unsigned char sharecount,
               unsigned char* shares,
               unsigned char* sharenrs)
{
  unsigned char *coefficients = alloca(threshold);
  unsigned int i, j, ilog;
  
  for( i = 0; i < threshold - 1; ++i )
    coefficients[i] = random() & 0xff;
  coefficients[threshold-1] = secret;
  
  for( i = 0; i < sharecount; ++i ) {
    ilog = logs[sharenrs[i] + 1]; /* sharenrs must be 0 based */
    shares[i] = coefficients[0];
    for( j = 1; j < threshold; ++j ) {
      unsigned char share = shares[i];
      if( share )
        share = exps[ilog + logs[share]]; /* share *= ilog */
      shares[i] = share ^ coefficients[j]; /* share += coefficient */
    }
  }
}

unsigned char 
bytewise_combine(unsigned char sharecount, /* threshold shares please */
                 unsigned char* shares,
                 unsigned char* sharenrs)
{
  unsigned char result = 0;
  unsigned int i, j;
  
  for( i = 0; i < sharecount; ++i ) {
    /* Compute L(i) as per Lagrange Interpolation */
    unsigned Li_top = 0, Li_bottom = 0;
    for( j = 0; j < sharecount; ++j ) {
      if( i == j ) continue;
      Li_top += logs[sharenrs[j] + 1];
      if( Li_top >= 0xff ) Li_top -= 0xff;
      Li_bottom += logs[(sharenrs[i] + 1) ^ (sharenrs[j] + 1)];
      if( Li_bottom >= 0xff ) Li_bottom -= 0xff;
    }
    if( Li_bottom  > Li_top ) Li_top += 0xff;
    Li_top -= Li_bottom; /* Li_top is now log(L(i)) */
    
    if( shares[i] )
      result ^= exps[Li_top + logs[shares[i]]];
  }
  
  return result;
}


