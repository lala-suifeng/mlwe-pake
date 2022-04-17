#ifndef POLYVEC_H
#define POLYVEC_H

#include "poly.h"

typedef struct 
{
    poly vec[MLWE_D];
}polyvec;

void polyvec_tobytes(unsigned char *r, const polyvec *p);
void polyvec_frombytes(polyvec *p, const unsigned char *r);

void polyvec_ntt(polyvec *r);
void polyvec_invntt(polyvec *r);

void polyvec_getnoise(polyvec *r, const unsigned char *seed, unsigned char nonce);
void hash_vec_frompw(polyvec *gamma, const unsigned char *pw, unsigned char nonce);

void polyvec_pointwise_acc(poly *r, const polyvec *a, const polyvec *b);
void polyvec_add(polyvec *r, const polyvec *a, const polyvec *b);




#endif