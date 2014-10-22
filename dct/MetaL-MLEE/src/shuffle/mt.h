#ifndef __MT19937_H
#define __MT19937_H


/* Period parameters */  
#define N 624
#define M 397
#define MATRIX_A 0x9908b0df   /* constant vector a */
#define UPPER_MASK 0x80000000 /* most significant w-r bits */
#define LOWER_MASK 0x7fffffff /* least significant r bits */

/* Tempering parameters */   
#define TEMPERING_MASK_B 0x9d2c5680
#define TEMPERING_MASK_C 0xefc60000
#define TEMPERING_SHIFT_U(y)  (y >> 11)
#define TEMPERING_SHIFT_S(y)  (y << 7)
#define TEMPERING_SHIFT_T(y)  (y << 15)
#define TEMPERING_SHIFT_L(y)  (y >> 18)

struct MTRANDSTATE {
  int mti;
  unsigned long mt[624];
} ;
      

typedef struct MTRANDSTATE * MTRANDHANDLE;

#ifdef CPP
#define ANSIC
#define MTRAND_EXTCDEF extern "C"
#else
#define MTRAND_EXTCDEF 
#endif
#ifdef ANSIC
MTRAND_EXTCDEF MTRANDHANDLE sgenrand(unsigned long);
MTRAND_EXTCDEF unsigned long genrand(MTRANDHANDLE);
MTRAND_EXTCDEF double genrandr(MTRANDHANDLE);
MTRAND_EXTCDEF double genrandr2(MTRANDHANDLE);
MTRAND_EXTCDEF void destroyrand(MTRANDHANDLE);
MTRAND_EXTCDEF void mt_read_randomstate(MTRANDHANDLE h, FILE * f) ;
MTRAND_EXTCDEF void mt_write_randomstate(MTRANDHANDLE h, FILE * f) ;
#else
MTRANDHANDLE sgenrand();
unsigned long genrand();
double genrandr();
double genrandr2();
void destroyrand();
void mt_read_randomstate();
void mt_write_randomstate();
#endif
#endif
