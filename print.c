#include "params.h"
#include "poly.h"
#include "polyvec.h"
#include <stdio.h>

void print_poly(char *s, poly *p)
{
    printf("%s :\n", s);
    for (int i = 0; i < MLWE_N; i++)
    {
        printf("%d ", p->coeffs[i]);
    }
    printf("\n");
    
}

void print_polyvec(char *s, polyvec *p)
{
    printf("%s : \n", s);
    for(int i = 0; i< MLWE_D; i++)
    {
        for(int j = 0; j<MLWE_N; j++)
        {
            printf("%d ", p->vec[i].coeffs[j]);
        }
        printf("\n");
    }
}