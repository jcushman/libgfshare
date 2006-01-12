/*
 * This file is Copyright Daniel Silverstone <dsilvers@digital-scurf.org> 2006
 */

#ifndef LIBGFSHARE_H
#define LIBGFSHARE_H




/* Very low-level calls which may go away */
void gfshare_ll_bytewise_split(unsigned char /* secret */,
                               unsigned char /* threshold */,
                               unsigned char /* sharecount */,
                               unsigned char* /* shares */,
                               unsigned char* /* sharenrs */);

unsigned char gfshare_ll_bytewise_combine(unsigned char /* sharecount */,
                                          unsigned char* /* shares */,
                                          unsigned char* /* sharenrs */);


#endif /* LIBGFSHARE_H */

