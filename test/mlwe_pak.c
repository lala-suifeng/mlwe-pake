#include "../polyvec.h"
#include "../poly.h"
#include "../pake.h"
#include "../params.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define NTESTS 10000

int main()
{
    unsigned char pw[32] = "12345678";
    unsigned char cid[32] = "12345678";
    unsigned char sid[32] = "87654321";
    unsigned char send_c0[PAKE_SENDC0];
    unsigned char send_s0[PAKE_SENDS0];
    unsigned char state_1[HASH_BYTES+3] ={0};
    unsigned char state_2[HASH_BYTES+3] = {0};
    unsigned char k_2_s[PAKE_VERIFY];
    unsigned char k_3_s[PAKE_VERIFY];
    unsigned char k_3_c[PAKE_VERIFY];
    unsigned char session_key_c[SESSION_KEY];
    unsigned char session_key_s[SESSION_KEY];

    polyvec gamma;
    polyvec s_c;
    int i;
    int r;
    srand((unsigned)time(NULL));
    r = rand() % NTESTS;
    printf("\n                         Parameters                         \n");
    printf("n = 256, d = %d, q = %d, eta = %d, m = 2, g = 64, l = 1895\n", MLWE_D, MLWE_Q, MLWE_ETA);
    // printf("----------------------------------------------------------------\n");
    for(i = 0; i< NTESTS; i++)
    {
        pake_c0(send_c0,&s_c, state_1, pw, cid, sid, &gamma);
        pake_s0(send_s0, send_c0, k_2_s, k_3_s, &gamma, sid, state_2);
        pake_c1(session_key_c, k_3_c, send_s0, &s_c, state_1);
        pake_s1(session_key_s, k_3_c, k_3_s, state_2);
        
        
        if(i == r)
        {   
            printf("----------------------------------------------------------------\n");
            printf("Sample session keys are randomly selected: \n");
            printf("NO.%d\n", i);
            printf("----------------------------------------------------------------\n");

            printf("session_key of Client: \n");
            for ( int j = 0; j < 32; j++)
            {
                printf("%02x", session_key_c[j]);
            }
            printf("\n");

            printf("session_key of Server: \n");
            for ( int j = 0; j < 32; j++)
            {
                printf("%02x", session_key_s[j]);
            }
            printf("\n");
            printf("----------------------------------------------------------------\n");
            
        }

        if(memcmp(session_key_c, session_key_s, 32))
        {
            printf("%d: session key consensus error!\n", i+1);
            return 0;
        }
        
    }
    printf("Number of test samples: %d \nResult: All tests passed! MLWE_PAK running successfully!\n", NTESTS);
    printf("----------------------------------------------------------------\n");
    
        // pake_c0(send_c0,&s_c, state_1, pw, cid, sid, &gamma);
        // pake_s0(send_s0, send_c0, k_2_s, k_3_s, &gamma, sid, state_2);
        // pake_c1(session_key_c, k_3_c, send_s0, &s_c, state_1);
        // pake_s1(session_key_s, k_3_c, k_3_s, state_2);
        // if(memcmp(session_key_c, session_key_s, 32))
        // {
        //     printf(" session key consensus error!\n");
        //     return 0;
        // }
        // printf("test pass!\n");
    
    return 0;
    
     
    
}
