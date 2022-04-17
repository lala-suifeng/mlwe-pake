#include "../poly.h"
#include "../polyvec.h"
#include "../pake.h"
#include "../params.h"
#include "../randombytes.h"
#include "../key_consensus.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#define NTESTS 1000

extern void gen_matrix(polyvec *a, const unsigned char *seed, int transposed);

unsigned char seed[SEED_BYTES] = {0};
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

    unsigned char pw[PW_BYTES] = "helloworld";
    unsigned char sid[ID_BYTES] = "12345678";
    unsigned char cid[ID_BYTES] = "12345678";

    unsigned char k_c[SESSION_KEY];
    unsigned char k_s[SESSION_KEY];

    unsigned char k_2_s[PAKE_VERIFY];
    unsigned char k_3_c[PAKE_VERIFY];
    unsigned char k_3_s[PAKE_VERIFY];

    struct timeval timeval_start, timeval_end;
    randombytes(seed, SEED_BYTES);

    printf("---------The number of loop = %d ---------\n", NTESTS);
    printf("-----------------Operation----------------\n");

    gettimeofday(&timeval_start, NULL);
    for ( i = 0; i < NTESTS; i++)
    {
        gen_matrix(matrix, seed, 0);
    }
    gettimeofday(&timeval_end, NULL);
    printf("The average time of gen_matrix:\t %.3lf us \n", ((timeval_end.tv_usec + timeval_end.tv_sec * 1000000) - (timeval_start.tv_sec * 1000000 + timeval_start.tv_usec)) / (NTESTS * 1.0));

    // for ( i = 0; i < NTESTS; i++)
    // {
    //     gettimeofday(&timeval_start, NULL);
    //     gen_matrix(matrix, seed, 0);
    //     gettimeofday(&timeval_end, NULL);
    //     time_us += (double) (timeval_end.tv_usec + (timeval_end.tv_sec - timeval_start.tv_sec) * 1000000 - timeval_start.tv_usec);
    // }
    //  printf("average time of gen_matrix in us: %lf \n", (time_us)/NTESTS);


    gettimeofday(&timeval_start, NULL);
    for ( i = 0; i < NTESTS; i++)
    {
       polyvec_getnoise(&pv,seed,0);
    }
    gettimeofday(&timeval_end, NULL);
    printf("The average time of polyvec_getnoise \t: %.3lf us \n", ((timeval_end.tv_usec + timeval_end.tv_sec * 1000000) - (timeval_start.tv_sec * 1000000 + timeval_start.tv_usec)) / (NTESTS * 1.0));

    gettimeofday(&timeval_start, NULL);
    for ( i = 0; i < NTESTS; i++)
    {
        poly_ntt(&p);
    }
    gettimeofday(&timeval_end, NULL);
    printf("The average time of poly_ntt \t: %.3lf us \n", ((timeval_end.tv_usec + timeval_end.tv_sec * 1000000) - (timeval_start.tv_sec * 1000000 + timeval_start.tv_usec)) / (NTESTS * 1.0));

    gettimeofday(&timeval_start, NULL);
    for ( i = 0; i < NTESTS; i++)
    {
        polyvec_ntt(&pv);
    }
    gettimeofday(&timeval_end, NULL);
    printf("The average time of polyvec_ntt \t: %.3lf us\n", ((timeval_end.tv_usec + timeval_end.tv_sec * 1000000) - (timeval_start.tv_sec * 1000000 + timeval_start.tv_usec)) / (NTESTS * 1.0));

    gettimeofday(&timeval_start, NULL);
    for ( i = 0; i < NTESTS; i++)
    {
        poly_invntt(&p);
    }
    gettimeofday(&timeval_end, NULL);
    printf("The average time of poly_invntt \t: %.3lf us\n", ((timeval_end.tv_usec + timeval_end.tv_sec * 1000000) - (timeval_start.tv_sec * 1000000 + timeval_start.tv_usec)) / (NTESTS * 1.0));

    gettimeofday(&timeval_start, NULL);
    for ( i = 0; i < NTESTS; i++)
    {
        polyvec_invntt(&pv);
    }
    gettimeofday(&timeval_end, NULL);
    printf("The average time of polyvec_invntt \t: %.3lf us\n", ((timeval_end.tv_usec + timeval_end.tv_sec * 1000000) - (timeval_start.tv_sec * 1000000 + timeval_start.tv_usec)) / (NTESTS * 1.0));
    
    gettimeofday(&timeval_start, NULL);
    for ( i = 0; i < NTESTS; i++)
    {
        okcn_con(&p, &v, key);
    }
    gettimeofday(&timeval_end, NULL);
    printf("The average time of okcn_con \t: %.3lf us\n", ((timeval_end.tv_usec + timeval_end.tv_sec * 1000000) - (timeval_start.tv_sec * 1000000 + timeval_start.tv_usec)) / (NTESTS * 1.0));

    gettimeofday(&timeval_start, NULL);
    for ( i = 0; i < NTESTS; i++)
    {
        okcn_rec(&p, &v, key);
    }
    gettimeofday(&timeval_end, NULL);
    printf("The average time of okcn_rec \t: %.3lf us\n", ((timeval_end.tv_usec + timeval_end.tv_sec * 1000000) - (timeval_start.tv_sec * 1000000 + timeval_start.tv_usec)) / (NTESTS * 1.0));

    printf("----------------Client/Server------------\n");
    gettimeofday(&timeval_start, NULL);
    for ( i = 0; i < NTESTS; i++)
    {
        pake_c0(sendc0, &pv, state_1, pw, cid, sid, &gamma);
    }
    gettimeofday(&timeval_end, NULL);
    printf("The average time of pake_c0 \t: %.3lf us\n", ((timeval_end.tv_usec + timeval_end.tv_sec * 1000000) - (timeval_start.tv_sec * 1000000 + timeval_start.tv_usec)) / (NTESTS * 1.0));

    gettimeofday(&timeval_start, NULL);
    for ( i = 0; i < NTESTS; i++)
    {
        pake_s0(sends0, sendc0, k_2_s, k_3_s, &gamma, sid, state_2);
    }
    gettimeofday(&timeval_end, NULL);
    printf("The average time of pake_s0 \t: %.3lf us\n", ((timeval_end.tv_usec + timeval_end.tv_sec * 1000000) - (timeval_start.tv_sec * 1000000 + timeval_start.tv_usec)) / (NTESTS * 1.0));

    gettimeofday(&timeval_start, NULL);
    for ( i = 0; i < NTESTS; i++)
    {
        pake_c1(k_c, k_3_c, sends0, &pv, state_1);
    }
    gettimeofday(&timeval_end, NULL);
    printf("The average time of pake_c1 \t: %.3lf us\n", ((timeval_end.tv_usec + timeval_end.tv_sec * 1000000) - (timeval_start.tv_sec * 1000000 + timeval_start.tv_usec)) / (NTESTS * 1.0));

    gettimeofday(&timeval_start, NULL);
    for ( i = 0; i < NTESTS; i++)
    {
        pake_s1(k_s, k_3_c, k_3_s, state_2);
    }
    gettimeofday(&timeval_end, NULL);
    printf("The average time of pake_s1 \t: %.3lf us\n", ((timeval_end.tv_usec + timeval_end.tv_sec * 1000000) - (timeval_start.tv_sec * 1000000 + timeval_start.tv_usec)) / (NTESTS * 1.0));


    return 0;
}
