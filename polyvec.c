#include <stdio.h>
#include "polyvec.h"
#include "fips202.h"
#include "cbd.h"
#include "reduce.h"

void polyvec_tobytes(unsigned char *r, const polyvec *a)
{
  int i;
  for(i=0;i<MLWE_D;i++)
    poly_tobytes(r+i*POLY_BYTES, &a->vec[i]);
}

void polyvec_frombytes(polyvec *r, const unsigned char *a)
{
  int i;
  for(i=0;i<MLWE_D;i++)
    poly_frombytes(&r->vec[i], a+i*POLY_BYTES);
}

void polyvec_ntt(polyvec *r)
{
  int i;
  for(i=0;i<MLWE_D;i++)
    poly_ntt(&r->vec[i]);
}

void polyvec_invntt(polyvec *r)
{
  int i;
  for(i=0;i<MLWE_D;i++)
    poly_invntt(&r->vec[i]);
}

void polyvec_getnoise(polyvec *r, const unsigned char *seed, unsigned char nonce)
{
  int i;
  for(i = 0; i< MLWE_D; i++)
  {
    poly_getnoise(&r->vec[i], seed, nonce++);
  }
}

void hash_vec_frompw(polyvec *gamma, const unsigned char *pw, unsigned char nonce)
{
  int i;
  for(i = 0; i< MLWE_D;i++)
  {
    hash_pw(gamma->vec+i, pw, nonce++);
  }
} 
void polyvec_pointwise_acc(poly *r, const polyvec *a, const polyvec *b)
{
  int i,j;
  uint16_t t;
  for(j=0;j<MLWE_N;j++)
  {
    t = montgomery_reduce(4613* (uint32_t)b->vec[0].coeffs[j]); // 4613 = 2^{2*18} % q
    r->coeffs[j] = montgomery_reduce(a->vec[0].coeffs[j] * t);
    for(i=1;i<MLWE_D;i++)
    {
      t = montgomery_reduce(4613* (uint32_t)b->vec[i].coeffs[j]);
      r->coeffs[j] += montgomery_reduce(a->vec[i].coeffs[j] * t);
    }
    r->coeffs[j] = barrett_reduce(r->coeffs[j]);
  }
}

void polyvec_add(polyvec *r, const polyvec *a, const polyvec *b)
{
  int i;
  for(i=0;i<MLWE_D;i++)
    poly_add(&r->vec[i], &a->vec[i], &b->vec[i]);

}


