#ifndef POLY_H
#define POLY_H

#include <stdint.h>
#include "params.h"                                    
typedef struct 
{
    uint16_t coeffs[MLWE_N];
}poly;

typedef struct 
{
    uint8_t coeffs[MLWE_N];
}micro_poly;

void poly_tobytes(unsigned char *r, const poly *a);
void poly_frombytes(poly *r, const unsigned char *a);

void poly_small_frombytes(micro_poly *v, const unsigned char *r);
void poly_small_to_bytes(unsigned char *r, const micro_poly *v);

void poly_getnoise(poly *r,const unsigned char *seed, unsigned char nonce);
void hash_pw(poly *r, const unsigned char *seed, unsigned char nonce);

void poly_ntt(poly *r);
void poly_invntt(poly *r);

void poly_add(poly *r, const poly *a, const poly *b);
void poly_sub(poly *r, const poly *a, const poly *b);


#endif