#ifndef NEXUSIO_CRYP_EC_H
#define NEXUSIO_CRYP_EC_H
/*****************************************************************************/
#include <stdlib.h>
#include <string.h>

#include "tommath.h"
/*****************************************************************************/
void NEXUSIO_Ecc_Encrypt(mp_int *GX, mp_int *GY, mp_int *QX, mp_int *QY, 
                 mp_int *aa, mp_int *pp, mp_int *nn, void *data, 
                 mp_int *rx, mp_int *ry, mp_int *cc);
/*****************************************************************************/
long NEXUSIO_Ecc_Decrypt(mp_int *GX, mp_int *GY, mp_int *aa, mp_int *bb, 
                 mp_int *pp, mp_int *sk, void *data, mp_int *rx, 
                 mp_int *ry, mp_int *cc);
/*****************************************************************************/
void NEXUSIO_Ecc_MakeKey(mp_int *QX, mp_int *QY, mp_int *GX, mp_int *GY,
                 mp_int *sk, mp_int *nn, mp_int *aa, mp_int *bb,
                 mp_int *pp);
/*****************************************************************************/
#endif//cryp_ec_H
