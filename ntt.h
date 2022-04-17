#ifndef NTT_H
#define NTT_H

#include "inttypes.h"

// extern uint16_t montgomery_reduce(uint32_t a);
// extern uint16_t barrett_reduce(uint16_t a);
// extern uint16_t freeze(uint16_t x);

extern uint16_t omegas_montgomery[];
extern uint16_t omegas_inv_bitrev_montgomery[];

extern uint16_t psis_bitrev_montgomery[];
extern uint16_t psis_inv_montgomery[];

void bitrev_vector(uint16_t* poly);
void mul_coefficients(uint16_t* poly, const uint16_t* factors);
void ntt(uint16_t * a, const uint16_t* omega);

#endif