#ifndef PARAMS_H
#define PARAMS_H

#define MLWE_N 256
#define MLWE_Q 7681
#define MLWE_Q_BITS 13
#define MLWE_D 4

#if (MLWE_D == 1)
#define MLWE_ETA 16
#elif (MLWE_D == 2)
#define MLWE_ETA 13
#elif (MLWE_D == 3)
#define MLWE_ETA 8
#elif (MLWE_D == 4)
#define MLWE_ETA 6
#else
#error "MLWE_D must be in {1,2,3,4}"
#endif


#define SEED_BYTES 32
#define PAKE_KEYBYTES 32

#define OKCN_M 2
#define OKCN_M_BITS 1
#define OKCN_G 64 //2^6
#define OKCN_G_BITS 6
#define BETA 7681
#define ALPHA 2
#define OKCN_Q_2 15362

#define ID_BYTES 32
#define PW_BYTES 32

#define SESSION_KEY 32 // 32*8 = 256bit
#define PAKE_VERIFY 32 //32*8 = 256bit(H_3)

#define POLY_BYTES 416 //(MLWE_N * MLWE_Q_BITS / 8)
#define POLYVEC_BYTES (MLWE_D * POLY_BYTES) //()
#define MICRO_POLY_BYTES 192 //(MLWE_N * OKCN_G_BITS / 8)


#define PAKE_SENDC0 (ID_BYTES + POLYVEC_BYTES + SEED_BYTES)//
#define PAKE_SENDS0 (POLYVEC_BYTES + MICRO_POLY_BYTES + PAKE_VERIFY)//
#define PAKE_SENDC1 (PAKE_VERIFY) //

#define HASH_BYTES (2*ID_BYTES + 3 * POLYVEC_BYTES + PAKE_KEYBYTES)




#endif
