#ifndef PAKE_H
#define PAKE_H

#include "poly.h"
#include "polyvec.h"

void encode_c0(unsigned char *r, const unsigned char *m, const unsigned char *seed, const unsigned char *cid);
void decode_c0(polyvec *m, unsigned char *seed, unsigned char *cid, const unsigned char *r);

void encode_s0(unsigned char *r, const unsigned char *y_s, const micro_poly *v,const unsigned char *k_2_s);
void decode_s0(unsigned char *y_s_bytes, polyvec *y_s, micro_poly *v, unsigned char *k_2_s, const unsigned char *r);

void pake_c0(unsigned char *send, polyvec *s, unsigned char *state, const unsigned char *pw, const unsigned char *cid, const unsigned char *sid,polyvec *gamma);
void pake_s0(unsigned char *send, const unsigned char *received, unsigned char *k_2_s, unsigned char *k_3_s, const polyvec *gamma, const unsigned char *sid, unsigned char *state);
void pake_c1(unsigned char *sharedkey, unsigned char *k_3_c, const unsigned char *received, const polyvec *s, unsigned char *state);
void pake_s1(unsigned char *sharedkey, const unsigned char *k_3_c, const unsigned char *k_3_s,unsigned char *state);

#endif