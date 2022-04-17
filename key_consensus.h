#ifndef KEY_CONSENSUS_H
#define KEY_CONSENSUS_H

#include "poly.h"
#include "polyvec.h"

void okcn_con(poly *sigma, micro_poly *v, unsigned char *key);
void okcn_rec(const poly *sigma, const micro_poly *v, unsigned char *key);

#endif
