#include "../poly.h"
#include "../polyvec.h"
#include "../pake.h"
#include "../cpucycles.h"
#include "../params.h"
#include "../randombytes.h"
#include "../key_consensus.h"
#include <stdlib.h>
#include <stdio.h>

#define NTESTS 10000

extern void gen_matrix(polyvec *a, const unsigned char *seed, int transposed);

static int cmp_llu(const void *a, const void *b)
{
    if(*(unsigned long long *)a < *(unsigned long long *)b) return -1;
    if(*(unsigned long long *)a > *(unsigned long long *)b) return 1;
    return 0;
}

static unsigned long long median(unsigned long long *l, size_t llen)
{
    qsort(l, llen, sizeof(unsigned long long), cmp_llu);

    if(llen % 2) return l[llen/2];
    else return (l[llen/2-1]+l[llen/2])/2;
}

static unsigned long long average(unsigned long long *t, size_t tlen)
{
    unsigned long long acc = 0;
    size_t i;
    for(i = 0; i<tlen;i++)
        acc += t[i];
    return (acc/tlen);
    // return 0;
}

static void print_results(const char *s, unsigned long long *t, size_t tlen)
{
    size_t i;
    printf("%s", s);
    for(i = 0;i<tlen-1;i++)
    {
        t[i] = t[i+1]-t[i];
    }

    printf("\n");
    printf("average:  %llu\n", average(t, tlen - 1));
    printf("median: %llu\n", median(t, tlen));
    
    printf("\n");
}

unsigned long long t[NTESTS]={0};
unsigned char seed[32] = {0};

int main()
{
    poly p;
    polyvec pv;
    polyvec matrix[MLWE_D];
    micro_poly v;
    polyvec gamma;

    unsigned char key[PAKE_KEYBYTES];
    int i;

    unsigned char sendc0[PAKE_SENDC0];
    unsigned char sends0[PAKE_SENDS0];
    unsigned char state_1[HASH_BYTES+3];
    unsigned char state_2[HASH_BYTES+3];

    unsigned char pw[PW_BYTES] = "12345678";
    unsigned char sid[ID_BYTES] = "12345678";
    unsigned char cid[ID_BYTES] = "12345678";

    unsigned char k_c[SESSION_KEY];
    unsigned char k_s[SESSION_KEY];

    unsigned char k_2_s[PAKE_VERIFY];
    unsigned char k_3_c[PAKE_VERIFY];
    unsigned char k_3_s[PAKE_VERIFY];

    randombytes(seed, SEED_BYTES);
    for ( i = 0; i < NTESTS; i++)
    {
        t[i] = cpucycles();
        gen_matrix(matrix, seed, 0);
    }
    print_results("gen_a:        ", t, NTESTS);

    for(i = 0; i<NTESTS;i++)
    {
        t[i] = cpucycles();
        poly_getnoise(&p, seed, 0);
    }
    print_results("getnoise_poly:     ", t, NTESTS);

    for ( i = 0; i < NTESTS; i++)
    {
        t[i] = cpucycles();
        polyvec_getnoise(&pv, seed, 0);
    }
    print_results("getnoise_polyvec: ", t, NTESTS);

    for(i = 0; i < NTESTS; i++)
    {
        t[i] = cpucycles();
        poly_ntt(&p);
    }
    print_results("poly_ntt: ", t, NTESTS);

    for (i = 0; i < NTESTS; i++)
    {
        t[i] = cpucycles();
        polyvec_ntt(&pv);
    }
    print_results("polyvec_ntt:", t, NTESTS);

    for(i = 0; i<NTESTS;i++)
    {
        t[i] = cpucycles();
        poly_invntt(&p);
    }
    print_results("poly_invntt: ", t, NTESTS);
    
    for (i = 0; i < NTESTS; i++)
    {
        t[i] = cpucycles();
        polyvec_invntt(&pv);
    }
    print_results("polyvec_invntt: ", t, NTESTS);

    for(i =0; i<NTESTS; i++)
    {
        t[i] = cpucycles();
        okcn_con(&p, &v, key);
    }
    print_results("okcn_con:", t, NTESTS);

    for (i = 0; i < NTESTS; i++)
    {
        t[i] = cpucycles();
        okcn_rec(&p, &v, key);
    }
    print_results("okcn_rec: ", t, NTESTS);

    for (i = 0; i < NTESTS; i++)
    {
        t[i] = cpucycles();
        pake_c0(sendc0, &pv, state_1, pw, cid, sid, &gamma);
    }
    print_results("pake_c0: ", t, NTESTS);

    for(i = 0; i< NTESTS; i++)
    {
        t[i] = cpucycles();
        pake_s0(sends0, sendc0, k_2_s, k_3_s, &gamma, sid, state_2);
    }
    print_results("pake_s0: ", t, NTESTS);

    for(i = 0; i< NTESTS;i++)
    {
        t[i] = cpucycles();
        pake_c1(k_c, k_3_c, sends0, &pv, state_1);
    }
    print_results("pake_c1: ", t, NTESTS);

    for(i = 0; i< NTESTS; i++)
    {
        t[i] = cpucycles();
        pake_s1(k_s, k_3_c, k_3_s, state_2);
    }
    print_results("pake_s1: ", t, NTESTS);


    


}

// int main()
// {
//     poly p;
//     unsigned char seed[32] = {0};
//     int i;
//     printf("test_noise_sample: \n");
//     poly_getnoise(&p, seed, 1);
//     for ( i = 0; i < 256; i++)
//     {
//         printf("%d ", p.coeffs[i]);
//     }
//     printf("\n");

//     return 0;
// }