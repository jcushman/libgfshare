/*
 * This file is Copyright Daniel Silverstone <dsilvers@digital-scurf.org> 2006
 */

#include "libgfshare.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int
main( int argc, char **argv )
{
  int ok = 1, i;
  unsigned char* secret = malloc(512);
  unsigned char* share1 = malloc(512);
  unsigned char* share2 = malloc(512);
  unsigned char* share3 = malloc(512);
  unsigned char* recomb = malloc(512);
  unsigned char* sharenrs = (unsigned char*)strdup("012");
  gfshare_ctx *G;
  
  /* Stage 1, make a secret */
  for( i = 0; i < 512; ++i )
    secret[i] = (random() & 0xff00) >> 8;
  /* Stage 2, split it three ways with a threshold of 2 */
  G = gfshare_ctx_init_enc( sharenrs, 3, 2, 512 );
  gfshare_ctx_enc_setsecret( G, secret );
  gfshare_ctx_enc_getshare( G, 0, share1 );
  gfshare_ctx_enc_getshare( G, 1, share2 );
  gfshare_ctx_enc_getshare( G, 2, share3 );
  gfshare_ctx_free( G );
  /* Prep the decode shape */
  G = gfshare_ctx_init_dec( sharenrs, 3, 512 );
  gfshare_ctx_dec_giveshare( G, 0, share1 );
  gfshare_ctx_dec_giveshare( G, 1, share2 );
  gfshare_ctx_dec_giveshare( G, 2, share3 );
  /* Stage 3, attempt a recombination with shares 1 and 2 */
  sharenrs[2] = 0;
  gfshare_ctx_dec_newshares( G, sharenrs );
  gfshare_ctx_dec_extract( G, recomb );
  for( i = 0; i < 512; ++i )
    if( secret[i] != recomb[i] ) 
      ok = 0;
  return ok!=1;
}
