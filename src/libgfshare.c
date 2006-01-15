/*
 * This file is Copyright Daniel Silverstone <dsilvers@digital-scurf.org> 2006
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */

#include "config.h"
#include "libgfshare.h"
#include "libgfshare_tables.h"

#include <stdlib.h>
#include <string.h>

#define XMALLOC malloc
#define XFREE free

struct _gfshare_ctx {
  unsigned int sharecount;
  unsigned int threshold;
  unsigned int size;
  unsigned char* sharenrs;
  unsigned char* buffer;
};

static void
_gfshare_fill_rand_using_random( unsigned char* buffer,
                                 unsigned int count )
{
  unsigned int i;
  for( i = 0; i < count; ++i )
    buffer[i] = (random() & 0xff00) >> 8; /* apparently the bottom 8 aren't
                                           * very random but the middles ones
                                           * are
                                           */
}

gfshare_rand_func_t gfshare_fill_rand = _gfshare_fill_rand_using_random;

/* ------------------------------------------------------[ Preparation ]---- */

/* Initialise a gfshare context for producing shares */
gfshare_ctx *
gfshare_ctx_init_enc( unsigned char* sharenrs,
                      unsigned int sharecount,
                      unsigned char threshold,
                      unsigned int size )
{
  gfshare_ctx *ctx = XMALLOC( sizeof(struct _gfshare_ctx) );
  ctx->sharecount = sharecount;
  ctx->threshold = threshold;
  ctx->size = size;
  ctx->sharenrs = XMALLOC( sharecount );
  memcpy( ctx->sharenrs, sharenrs, sharecount );
  ctx->buffer = XMALLOC( threshold * size );
  return ctx;
}

/* Initialise a gfshare context for recombining shares */
gfshare_ctx*
gfshare_ctx_init_dec( unsigned char* sharenrs,
                      unsigned int sharecount,
                      unsigned int size )
{
  gfshare_ctx *ctx = XMALLOC( sizeof(struct _gfshare_ctx) );
  ctx->sharecount = sharecount;
  ctx->threshold = 0;
  ctx->size = size;
  ctx->sharenrs = XMALLOC( sharecount );
  memcpy( ctx->sharenrs, sharenrs, sharecount );
  ctx->buffer = XMALLOC( (ctx->size * ctx->sharecount) );
  return ctx;
}

/* Free a share context's memory. (does not free the ctx itself) */
void 
gfshare_ctx_free( gfshare_ctx* ctx )
{
  XFREE( ctx->buffer );
  ctx->buffer = 0; /* Set buffer to zero in case we're called again */
}

/* --------------------------------------------------------[ Splitting ]---- */

/* Provide a secret to the encoder. (this re-scrambles the coefficients) */
void 
gfshare_ctx_enc_setsecret( gfshare_ctx* ctx,
                           unsigned char* secret)
{
  memcpy( ctx->buffer + ((ctx->threshold-1) * ctx->size),
          secret,
          ctx->size );
  gfshare_fill_rand( ctx->buffer, (ctx->threshold-1) * ctx->size );
}

/* Extract a share from the context. 
 * 'share' must be preallocated and at least 'size' bytes long.
 * 'sharenr' is the index into the 'sharenrs' array of the share you want.
 */
void 
gfshare_ctx_enc_getshare( gfshare_ctx* ctx,
                          unsigned char sharenr,
                          unsigned char* share)
{
  unsigned int pos, coefficient;
  unsigned int ilog = logs[ctx->sharenrs[sharenr]];
  unsigned char *coefficient_ptr = ctx->buffer;
  unsigned char *share_ptr;
  for( pos = 0; pos < ctx->size; ++pos )
    share[pos] = *(coefficient_ptr++);
  for( coefficient = 1; coefficient < ctx->threshold; ++coefficient ) {
    share_ptr = share;
    for( pos = 0; pos < ctx->size; ++pos ) {
      unsigned char share_byte = *share_ptr;
      if( share_byte )
        share_byte = exps[ilog + logs[share_byte]];
      *share_ptr++ = share_byte ^ *coefficient_ptr++;
    }
  }
}

/* ----------------------------------------------------[ Recombination ]---- */

/* Inform a recombination context of a change in share indexes */
void 
gfshare_ctx_dec_newshares( gfshare_ctx* ctx,
                           unsigned char* sharenrs)
{
  memcpy( ctx->sharenrs, sharenrs, ctx->sharecount );
}

/* Provide a share context with one of the shares.
 * The 'sharenr' is the index into the 'sharenrs' array
 */
void 
gfshare_ctx_dec_giveshare( gfshare_ctx* ctx,
                           unsigned char sharenr,
                           unsigned char* share )
{
  memcpy( ctx->buffer + (sharenr * ctx->size), share, ctx->size );
}

/* Extract the secret by interpolation of the shares.
 * secretbuf must be allocated and at least 'size' bytes long
 */
void
gfshare_ctx_dec_extract( gfshare_ctx* ctx,
                         unsigned char* secretbuf )
{
  unsigned int i, j;
  unsigned char *secret_ptr, *share_ptr;
  
  for( i = 0; i < ctx->size; ++i )
    secretbuf[i] = 0;
  
  for( i = 0; i < ctx->sharecount; ++i ) {
    /* Compute L(i) as per Lagrange Interpolation */
    unsigned Li_top = 0, Li_bottom = 0;
    
    if( ctx->sharenrs[i] == 0 ) continue; /* this share is not provided. */
    
    for( j = 0; j < ctx->sharecount; ++j ) {
      if( i == j ) continue;
      if( ctx->sharenrs[j] == 0 ) continue; /* skip empty share */
      Li_top += logs[ctx->sharenrs[j]];
      if( Li_top >= 0xff ) Li_top -= 0xff;
      Li_bottom += logs[(ctx->sharenrs[i]) ^ (ctx->sharenrs[j])];
      if( Li_bottom >= 0xff ) Li_bottom -= 0xff;
    }
    if( Li_bottom  > Li_top ) Li_top += 0xff;
    Li_top -= Li_bottom; /* Li_top is now log(L(i)) */
    
    secret_ptr = secretbuf; share_ptr = ctx->buffer + (ctx->size * i);
    for( j = 0; j < ctx->size; ++j ) {
      if( *share_ptr )
        *secret_ptr ^= exps[Li_top + logs[*share_ptr]];
      share_ptr++; secret_ptr++;
    }
  }
}

