 #include <stdio.h>
#include "poly.h"
#include "ntt.h"
#include "polyvec.h"
#include "reduce.h"
#include "cbd.h"
#include "fips202.h"


void poly_tobytes(unsigned char *r, const poly *a)
{
  int i,j;
  uint16_t t[8];

  for(i=0;i<MLWE_N/8;i++)
  {
    for(j=0;j<8;j++)
      t[j] = freeze(a->coeffs[8*i+j]);

    r[13*i+ 0] =  t[0]        & 0xff;
    r[13*i+ 1] = (t[0] >>  8) | ((t[1] & 0x07) << 5);
    r[13*i+ 2] = (t[1] >>  3) & 0xff;
    r[13*i+ 3] = (t[1] >> 11) | ((t[2] & 0x3f) << 2);
    r[13*i+ 4] = (t[2] >>  6) | ((t[3] & 0x01) << 7);
    r[13*i+ 5] = (t[3] >>  1) & 0xff;
    r[13*i+ 6] = (t[3] >>  9) | ((t[4] & 0x0f) << 4);
    r[13*i+ 7] = (t[4] >>  4) & 0xff;
    r[13*i+ 8] = (t[4] >> 12) | ((t[5] & 0x7f) << 1);
    r[13*i+ 9] = (t[5] >>  7) | ((t[6] & 0x03) << 6);
    r[13*i+10] = (t[6] >>  2) & 0xff;
    r[13*i+11] = (t[6] >> 10) | ((t[7] & 0x1f) << 3);
    r[13*i+12] = (t[7] >>  5);
  }
}

void poly_frombytes(poly *r, const unsigned char *a)
{
  int i;
  for(i=0;i<MLWE_N/8;i++)
  {
    r->coeffs[8*i+0] =  a[13*i+ 0]       | (((uint16_t)a[13*i+ 1] & 0x1f) << 8);
    r->coeffs[8*i+1] = (a[13*i+ 1] >> 5) | (((uint16_t)a[13*i+ 2]       ) << 3) | (((uint16_t)a[13*i+ 3] & 0x03) << 11);
    r->coeffs[8*i+2] = (a[13*i+ 3] >> 2) | (((uint16_t)a[13*i+ 4] & 0x7f) << 6);
    r->coeffs[8*i+3] = (a[13*i+ 4] >> 7) | (((uint16_t)a[13*i+ 5]       ) << 1) | (((uint16_t)a[13*i+ 6] & 0x0f) <<  9);
    r->coeffs[8*i+4] = (a[13*i+ 6] >> 4) | (((uint16_t)a[13*i+ 7]       ) << 4) | (((uint16_t)a[13*i+ 8] & 0x01) << 12);
    r->coeffs[8*i+5] = (a[13*i+ 8] >> 1) | (((uint16_t)a[13*i+ 9] & 0x3f) << 7);
    r->coeffs[8*i+6] = (a[13*i+ 9] >> 6) | (((uint16_t)a[13*i+10]       ) << 2) | (((uint16_t)a[13*i+11] & 0x07) << 10);
    r->coeffs[8*i+7] = (a[13*i+11] >> 3) | (((uint16_t)a[13*i+12]       ) << 5);
  }
}


void poly_getnoise(poly *r,const unsigned char *seed, unsigned char nonce)
{
  unsigned char buf[MLWE_ETA*MLWE_N/4]; // MLWE_N * (MLWE_ETA * 2) / 8
  unsigned char extseed[SEED_BYTES+1];
  int i;

  for(i=0;i<SEED_BYTES;i++)
    extseed[i] = seed[i];
  extseed[SEED_BYTES] = nonce;
     
  shake256(buf,MLWE_ETA*MLWE_N/4,extseed,SEED_BYTES+1);

  cbd(r, buf);
}


void poly_small_frombytes(micro_poly *v, const unsigned char *r)
{
  int i;
  for(i = 0; i< MLWE_N/4;i++)
  {
    v->coeffs[4*i + 0] = r[3*i + 0] & 0x3f;
    v->coeffs[4*i + 1] = (r[3*i + 0] >> 6) | ((r[3*i + 1] & 0x0f) << 2);
    v->coeffs[4*i + 2] = (r[3*i + 1] >> 4) | ((r[3*i  + 2] & 0x03) << 4);
    v->coeffs[4*i + 3] = r[3*i + 2] >> 2;  
  }
}

void poly_small_to_bytes(unsigned char *r, const micro_poly *v)
{
  int i;
  uint8_t m0, m1, m2, m3;
  for(i = 0; i < MLWE_N / 4; i++)
  {
    m0 = v->coeffs[4*i + 0];
    m1 = v->coeffs[4*i + 1];
    m2 = v->coeffs[4*i + 2];
    m3 = v->coeffs[4*i + 3];

    r[3*i + 0] = m0 | (m1 << 6);
    r[3*i + 1] = (m1 >> 2) | (m2 << 4);
    r[3*i + 2] = (m2 >> 4) | (m3 << 2);
  }
}


//FUN: gamma = H_1(PW_C)
void hash_pw(poly *a, const unsigned char *seed, unsigned char nonce)
{
  unsigned int pos = 0, ctr = 0;
  uint16_t val;
  unsigned int nblocks=4;
  uint8_t buf[SHAKE128_RATE*nblocks];
  int i;
  uint64_t state[25]; // CSHAKE state
  unsigned char extseed[SEED_BYTES+1];

  for(i=0;i<SEED_BYTES;i++)
    extseed[i] = seed[i];
  extseed[SEED_BYTES] = nonce;

  shake128_absorb(state,extseed,SEED_BYTES+1);
  shake128_squeezeblocks(buf,nblocks,state);

  while(ctr < MLWE_N)
  {
    val = (buf[pos] | ((uint16_t) buf[pos+1] << 8)) & 0x1fff;
    if(val < MLWE_Q)
    {
        a->coeffs[ctr++] = val;
    }
    pos += 2;

    if(pos > SHAKE128_RATE*nblocks-2)
    {
      nblocks = 1;
      shake128_squeezeblocks(buf,nblocks,state);
      pos = 0;
    }
  }

}


void poly_ntt(poly *r)
{
  mul_coefficients(r->coeffs, psis_bitrev_montgomery);
  ntt(r->coeffs, omegas_montgomery);
}

void poly_invntt(poly *r)
{
  bitrev_vector(r->coeffs);
  ntt(r->coeffs, omegas_inv_bitrev_montgomery);
  mul_coefficients(r->coeffs, psis_inv_montgomery);
}
  
void poly_add(poly *r, const poly *a, const poly *b)
{
  int i;
  for(i=0;i<MLWE_N;i++)
    r->coeffs[i] = barrett_reduce(a->coeffs[i] + b->coeffs[i]);
}

void poly_sub(poly *r, const poly *a, const poly *b)
{
  int i;
  for(i=0;i<MLWE_N;i++)
    r->coeffs[i] = barrett_reduce(a->coeffs[i] + 3*MLWE_Q - b->coeffs[i]);
}
