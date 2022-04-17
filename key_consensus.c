#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include "key_consensus.h"
#include "params.h"
#include "poly.h"
#include "polyvec.h"
#include <stdio.h>
#include "randombytes.h"
#include "reduce.h"

void okcn_con(poly *sigma, micro_poly *v, unsigned char *key)
{

    int i,j;
    poly e;
    unsigned char errorseed[SEED_BYTES];
    randombytes(errorseed, SEED_BYTES);
    poly_getnoise(&e, errorseed, 0);
    // printf("e.coeffs:\n");
    // for(i = 0; i< 256;i++){
    //     printf("%d ", e.coeffs[i] %2);
    // }
    // printf("\n");
    for(i=0;i<MLWE_N/8;i++){
      for(j=0;j<8;j++){

        uint16_t sigma_a;
        if(sigma->coeffs[8*i+j] > MLWE_Q) 
            sigma_a = ((sigma->coeffs[8*i+j] - MLWE_Q)<<1) + e.coeffs[8*i+j]%2;
        else
            sigma_a = (sigma->coeffs[8*i+j] << 1) + e.coeffs[8*i+j]%2;
        
        uint16_t v_;
        if(sigma_a<BETA){
            key[i]=key[i]<<1;
            v_=sigma_a;
        }
        else{
            key[i]=(key[i]<<1)|0x1;
            v_=sigma_a-BETA;
        }
        v->coeffs[8*i+j]=((uint32_t)v_<<6)/BETA;
        }
    }

}


void okcn_rec(const poly *sigma, const micro_poly *v, unsigned char *key)
{
    int i,j;
    // uint8_t k_;
    float r;
    for(i = 0; i<32;i++)
    {
        for(j= 0; j < 8; j++)
        {
                if(sigma->coeffs[8*i+j] > MLWE_Q)
                    r = ((sigma->coeffs[8*i+j] - MLWE_Q) << 1)/7681.0 - (v->coeffs[8*i+j]+0.5)/64;
                else
                    r = (sigma->coeffs[8*i+j] << 1)/7681.0 - (v->coeffs[8*i+j]+0.5)/64;

            
            // printf("r_%d %f", 8*i+j, r);
            if (-0.5 <= r && r<0.5)
                key[i] = key[i]<<1;
            else
                key[i] = (key[i] << 1) | 0x1;
            // printf(" key: %d\n", key[i]);
        }
             
    }
    
}

