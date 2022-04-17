#include <string.h>
#include "poly.h"
#include "polyvec.h"
#include "randombytes.h"
#include "key_consensus.h"
#include "fips202.h"
#include "pake.h"
#include <stdio.h>


void encode_c0(unsigned char *r, const unsigned char *m, const unsigned char *seed, const unsigned char *cid)
{
    int i;

    for ( i = 0; i < POLYVEC_BYTES; i++)
        r[i] = m[i];

    for ( i = 0; i < SEED_BYTES; i++)
        r[i + POLYVEC_BYTES ] = seed[i];

    for ( i = 0; i < ID_BYTES; i++)
        r[POLYVEC_BYTES + SEED_BYTES + i] = cid[i];

    // poly_tobytes()
}


void decode_c0(polyvec *m, unsigned char *seed, unsigned char *cid, const unsigned char *r)
{
    int i;
    polyvec_frombytes(m, r);
    for(i = 0; i<SEED_BYTES; i++)
        seed[i] = r[i + POLYVEC_BYTES];
    for(i = 0; i<ID_BYTES;i++)
        cid[i] = r[i + POLYVEC_BYTES + SEED_BYTES];

}

void encode_s0(unsigned char *r, const unsigned char *y_s, const micro_poly *v,const unsigned char *k_2_s)
{
    int i;
    poly_small_to_bytes(r,v);
    for(i = 0; i< POLYVEC_BYTES; i++)
        r[i + MICRO_POLY_BYTES] = y_s[i];
    // for(i = 0; i< MICRO_POLY_BYTES; i++)
    //     r[i + POLYVEC_BYTES] = v[i];
    for(i = 0; i< PAKE_VERIFY; i++)
        r[i + POLYVEC_BYTES + MICRO_POLY_BYTES] =  k_2_s[i];
}

void decode_s0(unsigned char *y_s_bytes, polyvec *y_s, micro_poly *v, unsigned char *k_2_s, const unsigned char *r)
{
    int i;
    poly_small_frombytes(v, r);
    for(i = 0; i< POLYVEC_BYTES; i++)
        y_s_bytes[i] = r[i + MICRO_POLY_BYTES];
    polyvec_frombytes(y_s, y_s_bytes);
    for(i = 0; i< PAKE_VERIFY;i++)
        k_2_s[i] = r[i + MICRO_POLY_BYTES + POLYVEC_BYTES];
 
}

void gen_matrix(polyvec *a, const unsigned char *seed, int transposed) //XXX: Not static for benchmarking
{
  unsigned int pos=0, ctr;
  uint16_t val;
  unsigned int nblocks=4;
  uint8_t buf[SHAKE128_RATE*nblocks];
  int i,j;
  uint64_t state[25]; // CSHAKE state
  unsigned char extseed[SEED_BYTES+2];

  for(i=0;i<SEED_BYTES;i++)
    extseed[i] = seed[i];


  for(i=0;i<MLWE_D;i++)
  {
    for(j=0;j<MLWE_D;j++)
    {
      ctr = pos = 0;
      if(transposed) 
      {
        extseed[SEED_BYTES]   = i;
        extseed[SEED_BYTES+1] = j;
      }
      else
      {
        extseed[SEED_BYTES]   = j;
        extseed[SEED_BYTES+1] = i;
      }
        
      shake128_absorb(state,extseed,SEED_BYTES+2);
      shake128_squeezeblocks(buf,nblocks,state);

      while(ctr < MLWE_N)
      {
        val = (buf[pos] | ((uint16_t) buf[pos+1] << 8)) & 0x1fff;
        if(val < MLWE_Q)
        {
            a[i].vec[j].coeffs[ctr++] = val;
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
  }
}
//api

void pake_c0(unsigned char *send, polyvec *s_c, unsigned char *state, const unsigned char *pw, const unsigned char *cid, const unsigned char *sid,polyvec *gamma)
{
    polyvec A[MLWE_D];
    polyvec e_c, m;
    polyvec y_c;
    unsigned char seed[SEED_BYTES];
    unsigned char noiseseed_c[SEED_BYTES];
    unsigned char nonce = 0;
    int i;

    //compute y_c = As+e
    randombytes(seed, SEED_BYTES);
    randombytes(noiseseed_c, SEED_BYTES);

    gen_matrix(A, seed, 0);

    polyvec_getnoise(s_c,noiseseed_c, nonce);
    polyvec_ntt(s_c);

    polyvec_getnoise(&e_c, noiseseed_c, nonce);
    polyvec_ntt(&e_c);

    for ( i = 0; i < MLWE_D; i++)
        polyvec_pointwise_acc(y_c.vec+i, s_c, A+i);

    polyvec_add(&y_c, &y_c, &e_c);
    int j; 
    
    hash_vec_frompw(gamma, pw, nonce);
    polyvec_add(&m, &y_c, gamma);
    
   
    for(i = 0; i< MLWE_D; i++)
        for (j = 0; j < MLWE_N; j++)
            gamma->vec[i].coeffs[j] = MLWE_Q - gamma->vec[i].coeffs[j];

    // polyvec_add(&y_c, &m, gamma);
    // printf("y_c_gamma:\n");
    // for(i= 0;i< 3;i++)
    // {
    //     for(j = 0;j<256;j++)
    //     {
    //         printf("%d ", y_c.vec[i].coeffs[j]);
    //     }
    //     printf("\n");
    // }


    for (i = 0; i < ID_BYTES; i++)
    {
        state[i] = cid[i];
        state[i + ID_BYTES] = sid[i];
    }
    unsigned char mbytes[POLYVEC_BYTES];
    polyvec_tobytes(mbytes, &m);
    for (i = 0; i < POLYVEC_BYTES; i++)
        state[i+2*ID_BYTES] = mbytes[i];

    unsigned char gammabytes[POLYVEC_BYTES];
    
    polyvec_tobytes(gammabytes, gamma);
    for (i = 0; i < POLYVEC_BYTES; i++)
        state[i+2*ID_BYTES+2*POLYVEC_BYTES+PAKE_KEYBYTES] = gammabytes[i];


    encode_c0(send, mbytes, seed, cid);  
    
    // poly_getnoise(&e, noiseseed, 0);

}

void pake_s0(unsigned char *send, const unsigned char *received, unsigned char *k_2_s, unsigned char *k_3_s, const polyvec *gamma, const unsigned char *sid, unsigned char *state)
{
    polyvec s_s, e_s, AT[MLWE_D], m;
    polyvec y_s, y_c;
    poly sigma_s, e_sigma;
    micro_poly v;
    unsigned char seed[SEED_BYTES];
    unsigned char noiseseed_s[SEED_BYTES];
    unsigned char cid[ID_BYTES];
    unsigned char key_s[PAKE_KEYBYTES];
    // unsigned char state[HASH_BYTES+3];
    // unsigned char k_3_s[PAKE_VERIFY];
    int nonce = 0;
    int i, j;
    
    randombytes(noiseseed_s, SEED_BYTES);

    decode_c0(&m, seed, cid, received);
    
    gen_matrix(AT, seed, 1);//gen_at

    polyvec_getnoise(&s_s, noiseseed_s, nonce);
    polyvec_ntt(&s_s);

    polyvec_getnoise(&e_s, noiseseed_s, nonce);
    polyvec_ntt(&e_s);

    for (i = 0; i < MLWE_D; i++)
        polyvec_pointwise_acc(y_s.vec+i, &s_s, AT+i);
    
    polyvec_add(&y_s, &y_s, &e_s);

    // print_polyvec("y_s ", &y_s);

    

    // polyvec_add(&y_c, &m, gamma);
    // print_polyvec("y_c ", &y_c);for(i = 0; i< MLWE_D; i++)
    for(i = 0; i< MLWE_D; i++)
        for (j = 0; j < MLWE_N; j++)
            y_c.vec[i].coeffs[j]  = (m.vec[i].coeffs[j] + gamma->vec[i].coeffs[j]) % MLWE_Q;
    
    poly_getnoise(&e_sigma, noiseseed_s, nonce);
    polyvec_pointwise_acc(&sigma_s, &s_s, &y_c);
    poly_invntt(&sigma_s);
    poly_add(&sigma_s, &sigma_s, &e_sigma);

    // print_poly("sigma_s ", &sigma_s);

    okcn_con(&sigma_s, &v, key_s);

    // printf("key_s: \n");
    // for(i = 0; i< 32;i++)
    // {
    //     printf("%02x ", key_s[i]);
    // }
    // printf("\n");

    for(i = 0; i<ID_BYTES; i++){
        state[i] = cid[i];
        state[i + ID_BYTES] = sid[i];
    }

    unsigned char mbytes[POLYVEC_BYTES];
    polyvec_tobytes(mbytes, &m);
    for (i = 0; i < POLYVEC_BYTES; i++)
        state[i + ID_BYTES *2] = mbytes[i];
    
    unsigned char y_s_bytes[POLYVEC_BYTES];
    polyvec_tobytes(y_s_bytes, &y_s);
    for(i = 0; i< POLYVEC_BYTES; i++)
        state[i + ID_BYTES *2 + POLYVEC_BYTES] = y_s_bytes[i];
    
    for(i = 0; i< PAKE_KEYBYTES; i++)
        state[i + ID_BYTES * 2 + POLYVEC_BYTES *2] = key_s[i];
    
    unsigned char gammabytes[POLYVEC_BYTES];
    polyvec_tobytes(gammabytes, gamma);
    for (i = 0; i < POLYVEC_BYTES; i++)
        state[i + ID_BYTES *2 + POLYVEC_BYTES *2 + PAKE_KEYBYTES] = gammabytes[i];
    
    // unsigned char k[PAKE_VERIFY];
    // #ifndef STATISTICAL_TEST
    shake256(k_2_s, PAKE_VERIFY, state, HASH_BYTES);
    // printf("k_2_s: \n");
    // for(i = 0; i<PAKE_VERIFY; i++)
    // {
    //     printf("%02x ", k_2_s[i]);
    // }
    // printf("\n");
    state[HASH_BYTES] = 0;
    shake256(k_3_s,PAKE_VERIFY, state, HASH_BYTES+1);
    // #endif

    encode_s0(send, y_s_bytes, &v,k_2_s);

}

void pake_c1(unsigned char *sharedkey, unsigned char *k_3_c, const unsigned char *received, const polyvec *s_c, unsigned char *state)
{
    poly sigma_c;
    polyvec y_s;
    micro_poly v;
    unsigned char key_c[PAKE_KEYBYTES];
    unsigned char k_2_s[PAKE_VERIFY];
    unsigned char k_2_c[PAKE_VERIFY];
    unsigned char y_s_bytes[POLYVEC_BYTES];
    int i;
    
    decode_s0(y_s_bytes, &y_s, &v, k_2_s, received);


    // print_polyvec("y_s ", &y_s);

    polyvec_pointwise_acc(&sigma_c, &y_s, s_c);
    poly_invntt(&sigma_c);
    // print_poly("sigma_c",&sigma_c);
    okcn_rec(&sigma_c, &v, key_c);
    // printf("key_c: \n");
    // for(i = 0; i< 32; i++)
    // {
    //     printf("%02x ", key_c[i]);
    // }
    // printf("\n");

    for(i = 0;i<POLYVEC_BYTES;i++)
        state[i + 2*ID_BYTES + POLYVEC_BYTES] = y_s_bytes[i];
    for(i = 0; i < PAKE_KEYBYTES; i++)
        state[i + 2*ID_BYTES + 2*POLYVEC_BYTES] = key_c[i];
    
    // unsigned char k_2_c[PAKE_VERIFY];

    // #ifndef STATISTICAL_TEST
    shake256(k_2_c, PAKE_VERIFY, state, HASH_BYTES);

    // printf("k_2_c: \n");
    // for(i = 0; i< PAKE_VERIFY;i++)
    // {
    //     printf("%02x ", k_2_c[i]);
    // }
    // printf("\n");
    // #endif

    if(memcmp(k_2_s, k_2_c, 32) == 0)
    {
        state[HASH_BYTES] = 0;
        shake256(k_3_c, PAKE_VERIFY, state, HASH_BYTES+1);
        state[HASH_BYTES+1] = 1;
        shake256(sharedkey, SESSION_KEY, state, HASH_BYTES+2);
    } 
}

void pake_s1(unsigned char *sharedkey, const unsigned char *k_3_c, const unsigned char *k_3_s, unsigned char *state)
{
    if ( memcmp(k_3_c, k_3_s, 32) == 0 )
    {
        state[HASH_BYTES+1]  = 1;
        shake256(sharedkey,SESSION_KEY, state, HASH_BYTES+2);
    }
    
}





