#include "cbd.h"

static uint64_t load_littleendian(const unsigned char *x, int bytes)
{
  int i;
  uint64_t r = x[0];
  for(i=1;i<bytes;i++)
    r |= (uint64_t)x[i] << (8*i);
  return r;
}

void cbd(poly *r, const unsigned char *buf)
{
  #if MLWE_ETA == 8

  uint64_t t, a[4], b[4];
  uint64_t d;
  int i, j;

  for (i = 0; i < MLWE_N/4; i++)
  {
    t = load_littleendian(buf+8*i, 8);
    d = 0;
    
    for(j = 0; j< 8;j++)
      d += (t>>j) & 0x0101010101010101;

    a[0] = d & 0xff;
    b[0] = (d >> 8) & 0xff;
    a[1] = (d >> 16) & 0xff;
    b[1] = (d >> 24) & 0xff;
    a[2] = (d >> 32) & 0xff;
    b[2] = (d >> 40) & 0xff;
    a[3] = (d >> 48) & 0xff;
    b[3] = (d >> 56);

    r->coeffs[4*i+0] = a[0] + MLWE_Q - b[0];
    r->coeffs[4*i+1] = a[1] + MLWE_Q - b[1];
    r->coeffs[4*i+2] = a[2] + MLWE_Q - b[2];
    r->coeffs[4*i+3] = a[3] + MLWE_Q - b[3];

  }
  #elif MLWE_ETA == 4
    uint32_t t,d, a[4], b[4];
  int i,j;

  for(i=0;i<MLWE_N/4;i++)
  {
    t = load_littleendian(buf+4*i,4);
    d = 0;
    for(j=0;j<4;j++)
      d += (t >> j) & 0x11111111; //count 1 of a 4-bit unit

    a[0] =  d & 0xf;
    b[0] = (d >>  4) & 0xf;
    a[1] = (d >>  8) & 0xf;
    b[1] = (d >> 12) & 0xf;
    a[2] = (d >> 16) & 0xf;
    b[2] = (d >> 20) & 0xf;
    a[3] = (d >> 24) & 0xf;
    b[3] = (d >> 28);

    r->coeffs[4*i+0] = a[0] + MLWE_Q - b[0];
    r->coeffs[4*i+1] = a[1] + MLWE_Q - b[1];
    r->coeffs[4*i+2] = a[2] + MLWE_Q - b[2];
    r->coeffs[4*i+3] = a[3] + MLWE_Q - b[3];
  }
  #elif MLWE_ETA == 16
  // uint64_t t[2]; 
  // uint64_t a[4],b[4];
  // // uint64_t d[2];
  // uint64_t d;
  // int i,j;

  // for ( i = 0; i < MLWE_N/4; i++)
  // {
  //   t[0] = load_littleendian(buf+ 16 * i, 8);
  //   t[1] = load_littleendian(buf + 16* i + 8, 8);
  //   // d[0] = d[1] = 0;
  //   d = 0;

  //   for (j = 0; j < 8 ; j++)
  //   {
  //     d += ((t[0] >> j) + (t[1] >> j)) & 0x0101010101010101; 
  //   }
  //   a[0] = d & 0xff;
  //   b[0] = (d >> 8) & 0xff;
  //   a[1] = (d >> 16) & 0xff;
  //   b[1] = (d >> 24) & 0xff;
  //   a[2] = (d >> 32) & 0xff;
  //   b[2] = (d >> 40) & 0xff;
  //   a[3] = (d >> 48) & 0xff;
  //   b[3] = (d >> 56);

  //   r->coeffs[4*i+0] = a[0] + MLWE_Q - b[0];
  //   r->coeffs[4*i+1] = a[1] + MLWE_Q - b[1];
  //   r->coeffs[4*i+2] = a[2] + MLWE_Q - b[2];
  //   r->coeffs[4*i+3] = a[3] + MLWE_Q - b[3];
  // }
  uint64_t t, a[4], b[4];
  uint64_t d;
  int i, j;

  for (i = 0; i < MLWE_N/2; i++)
  {
    t = load_littleendian(buf+8*i, 8);
    d = 0;
    
    for(j = 0; j< 8;j++)
      d += (t>>j) & 0x0101010101010101;

    a[0] = d & 0xff;
    b[0] = (d >> 8) & 0xff;
    a[1] = (d >> 16) & 0xff;
    b[1] = (d >> 24) & 0xff;
    a[2] = (d >> 32) & 0xff;
    b[2] = (d >> 40) & 0xff;
    a[3] = (d >> 48) & 0xff;
    b[3] = (d >> 56);


    r->coeffs[2*i+0] = (a[0] + a[1]) + MLWE_Q - (b[0] + b[1]);
    r->coeffs[2*i+1] = (a[2] + a[3]) + MLWE_Q - (b[2] + b[3]);

  }
  #elif MLWE_ETA == 13
  uint64_t t, a[4], b[4];
  uint64_t d;
  int i, j;

  for (i = 0; i < MLWE_N/4; i++)
  {
    t = load_littleendian(buf+8*i, 8);
    d = 0;
    
    for(j = 0; j< 8;j++)
      d += (t>>j) & 0x0101010101010101;

    a[0] = d & 0xff;
    b[0] = (d >> 8) & 0xff;
    a[1] = (d >> 16) & 0xff;
    b[1] = (d >> 24) & 0xff;
    a[2] = (d >> 32) & 0xff;
    b[2] = (d >> 40) & 0xff;
    a[3] = (d >> 48) & 0xff;
    b[3] = (d >> 56);

    r->coeffs[4*i+0] = a[0] + MLWE_Q - b[0];
    r->coeffs[4*i+1] = a[1] + MLWE_Q - b[1];
    r->coeffs[4*i+2] = a[2] + MLWE_Q - b[2];
    r->coeffs[4*i+3] = a[3] + MLWE_Q - b[3];
  }
  for ( i = 0; i < MLWE_N/4; i++)
  {
    t = load_littleendian(buf + 5 * i, 5);
    d = 0;

    for (j = 0; j < 5; j++)
      d += (t >> j) & 0x0842108421UL;
    
    a[0] =  d & 0x1f;
    b[0] = (d >>  5) & 0x1f;
    a[1] = (d >> 10) & 0x1f;
    b[1] = (d >> 15) & 0x1f;
    a[2] = (d >> 20) & 0x1f;
    b[2] = (d >> 25) & 0x1f;
    a[3] = (d >> 30) & 0x1f;
    b[3] = (d >> 35);

    r->coeffs[4*i+0] += a[0] - b[0];
    r->coeffs[4*i+1] += a[1] - b[1];
    r->coeffs[4*i+2] += a[2] - b[2];
    r->coeffs[4*i+3] += a[3] - b[3];
    
  }
  
  

  #elif MLWE_ETA == 6
  uint64_t t,d,a[4],b[4];
  int i, j;
  
  for ( i = 0; i < MLWE_N/4; i++)
  {
    t = load_littleendian(buf + 6 *i,6);
    d = 0;
    for (j = 0; j < 6; j++)
      d += (t >> j) & 0x041041041041UL;

    a[0] = d & 0x3f;
    b[0] = (d >> 6) & 0x3f;
    a[1] = (d >> 12) & 0x3f;
    b[1] = (d >> 18) & 0x3f;
    a[2] = (d >> 24) & 0x3f;
    b[2] = (d >> 30) & 0x3f;
    a[3] = (d >> 36) & 0x3f;
    b[3] = (d >> 42);

    r->coeffs[4*i + 0] = a[0] + MLWE_Q -b[0];
    r->coeffs[4*i + 1] = a[1] + MLWE_Q -b[1];
    r->coeffs[4*i + 2] = a[2] + MLWE_Q -b[2];
    r->coeffs[4*i + 3] = a[3] + MLWE_Q -b[3];
  }
  
  
  #endif
  // #elif MLWE_ETA == 8
  // uint64_t t, d;
  // uint32_t a[4], b[4];
  // int i,j;
  
  // for ( i = 0; i < MLWE_N/4 ; i++)
  // {
  //     t = load_littleendian(buf+8*i, 8);
  //     d = 0;
  //     for (j = 0; j < 4; j+=2)
  //     {
        
  //     }

      
  // }
  

  

}

// void cbd(poly *r, const unsigned char *buf)
// {
// #if KYBER_ETA == 3
//   uint32_t t,d, a[4], b[4];
//   int i,j;

//   for(i=0;i<KYBER_N/4;i++)
//   {
//     t = load_littleendian(buf+3*i,3);
//     d = 0;
//     for(j=0;j<3;j++)
//       d += (t >> j) & 0x249249;

//     a[0] =  d & 0x7;
//     b[0] = (d >>  3) & 0x7;
//     a[1] = (d >>  6) & 0x7;
//     b[1] = (d >>  9) & 0x7;
//     a[2] = (d >> 12) & 0x7;
//     b[2] = (d >> 15) & 0x7;
//     a[3] = (d >> 18) & 0x7;
//     b[3] = (d >> 21);

//     r->coeffs[4*i+0] = a[0] + KYBER_Q - b[0];
//     r->coeffs[4*i+1] = a[1] + KYBER_Q - b[1];
//     r->coeffs[4*i+2] = a[2] + KYBER_Q - b[2];
//     r->coeffs[4*i+3] = a[3] + KYBER_Q - b[3];
//   }
// #elif KYBER_ETA == 4
//   uint32_t t,d, a[4], b[4];
//   int i,j;

//   for(i=0;i<KYBER_N/4;i++)
//   {
//     t = load_littleendian(buf+4*i,4);
//     d = 0;
//     for(j=0;j<4;j++)
//       d += (t >> j) & 0x11111111;

//     a[0] =  d & 0xf;
//     b[0] = (d >>  4) & 0xf;
//     a[1] = (d >>  8) & 0xf;
//     b[1] = (d >> 12) & 0xf;
//     a[2] = (d >> 16) & 0xf;
//     b[2] = (d >> 20) & 0xf;
//     a[3] = (d >> 24) & 0xf;
//     b[3] = (d >> 28);

//     r->coeffs[4*i+0] = a[0] + KYBER_Q - b[0];
//     r->coeffs[4*i+1] = a[1] + KYBER_Q - b[1];
//     r->coeffs[4*i+2] = a[2] + KYBER_Q - b[2];
//     r->coeffs[4*i+3] = a[3] + KYBER_Q - b[3];
//   }
// #elif KYBER_ETA == 5
//   /*
//   uint32_t t,d, a[4], b[4];
//   int i,j;

//   for(i=0;i<KYBER_N/4;i++)
//   {
//     t = load_littleendian(buf+4*i,4);
//     d = 0;
//     for(j=0;j<4;j++)
//       d += (t >> j) & 0x11111111;

//     a[0] =  d & 0xf;
//     b[0] = (d >>  4) & 0xf;
//     a[1] = (d >>  8) & 0xf;
//     b[1] = (d >> 12) & 0xf;
//     a[2] = (d >> 16) & 0xf;
//     b[2] = (d >> 20) & 0xf;
//     a[3] = (d >> 24) & 0xf;
//     b[3] = (d >> 28);

//     r->coeffs[4*i+0] = a[0] + KYBER_Q - b[0];
//     r->coeffs[4*i+1] = a[1] + KYBER_Q - b[1];
//     r->coeffs[4*i+2] = a[2] + KYBER_Q - b[2];
//     r->coeffs[4*i+3] = a[3] + KYBER_Q - b[3];
//   }
//   */
//   uint64_t t,d, a[4], b[4];
//   int i,j;

//   for(i=0;i<KYBER_N/4;i++)
//   {
//     t = load_littleendian(buf+5*i,5);
//     d = 0;
//     for(j=0;j<5;j++)
//       d += (t >> j) & 0x0842108421UL;

//     a[0] =  d & 0x1f;
//     b[0] = (d >>  5) & 0x1f;
//     a[1] = (d >> 10) & 0x1f;
//     b[1] = (d >> 15) & 0x1f;
//     a[2] = (d >> 20) & 0x1f;
//     b[2] = (d >> 25) & 0x1f;
//     a[3] = (d >> 30) & 0x1f;
//     b[3] = (d >> 35);

//     r->coeffs[4*i+0] = a[0] + KYBER_Q - b[0];
//     r->coeffs[4*i+1] = a[1] + KYBER_Q - b[1];
//     r->coeffs[4*i+2] = a[2] + KYBER_Q - b[2];
//     r->coeffs[4*i+3] = a[3] + KYBER_Q - b[3];
//   }
// #else
// #error "poly_getnoise in poly.c only supports etain {3,4,5}"
// #endif
// }
