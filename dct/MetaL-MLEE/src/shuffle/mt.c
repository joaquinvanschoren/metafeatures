/* This version of the MT random number generator has been
   adopted from the three original files mt19937-2.c,
   mt19937.c, and mt19937int.c to integrate the generators
   for doubles from [0,1], double from [0,1), and integers
   into a single source. The seeding function sgenrand os
   common to all.
   genrand will return the random number as integer
   genrandr will return as double from [0,1]
   genrandr2 will return as double from [0,1)
   Below are the comments from all three sources...
   Other changes: make C++ conforming, remove main
   Author of adaptation: Johann Petrak
*/

/* A C-program for MT19937: Real number version                */
/*   genrand() generates one pseudorandom real number (double) */
/* which is uniformly distributed on [0,1]-interval, for each  */
/* call. sgenrand(seed) set initial values to the working area */
/* of 624 words. Before genrand(), sgenrand(seed) must be      */
/* called once. (seed is any 32-bit integer except for 0).     */
/* Integer generator is obtained by modifying two lines.       */
/*   Coded by Takuji Nishimura, considering the suggestions by */
/* Topher Cooper and Marc Rieffel in July-Aug. 1997.           */

/* A C-program for MT19937: Real number version([0,1)-interval)*/
/*   genrand() generates one pseudorandom real number (double) */
/* which is uniformly distributed on [0,1)-interval, for each  */
/* call. sgenrand(seed) set initial values to the working area */
/* of 624 words. Before genrand(), sgenrand(seed) must be      */
/* called once. (seed is any 32-bit integer except for 0).     */
/* Integer generator is obtained by modifying two lines.       */
/*   Coded by Takuji Nishimura, considering the suggestions by */
/* Topher Cooper and Marc Rieffel in July-Aug. 1997.           */

/* A C-program for MT19937: Integer     version                   */
/*  genrand() generates one pseudorandom unsigned integer (32bit) */
/* which is uniformly distributed among 0 to 2^32-1  for each     */
/* call. sgenrand(seed) set initial values to the working area    */
/* of 624 words. Before genrand(), sgenrand(seed) must be         */
/* called once. (seed is any 32-bit integer except for 0).        */
/*   Coded by Takuji Nishimura, considering the suggestions by    */
/* Topher Cooper and Marc Rieffel in July-Aug. 1997.              */


/* This library is free software; you can redistribute it and/or   */
/* modify it under the terms of the GNU Library General Public     */
/* License as published by the Free Software Foundation; either    */
/* version 2 of the License, or (at your option) any later         */
/* version.                                                        */
/* This library is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of  */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.            */
/* See the GNU Library General Public License for more details.    */
/* You should have received a copy of the GNU Library General      */
/* Public License along with this library; if not, write to the    */
/* Free Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA   */ 
/* 02111-1307  USA                                                 */

/* Copyright (C) 1997 Makoto Matsumoto and Takuji Nishimura.       */
/* When you use this, send an email to: matumoto@math.keio.ac.jp   */
/* with an appropriate reference to your work.                     */
#include<malloc.h>
#include<assert.h>
#include<stdio.h>
#include<sys/time.h>
#include "mt.h"

/* static unsigned long mt[N]; */ /* the array for the state vector  */
/* static int mti=N+1; */ /* mti==N+1 means mt[N] is not initialized */

/* initializing the array with a NONZERO seed */
#ifdef ANSIC
MTRANDHANDLE sgenrand(unsigned long seed)
#else
MTRANDHANDLE sgenrand(seed)
unsigned long seed;
#endif
{
    /* setting initial seeds to mt[N] using         */
    /* the generator Line 25 of Table 1 in          */
    /* [KNUTH 1981, The Art of Computer Programming */
    /*    Vol. 2 (2nd Ed.), pp102]                  */
  MTRANDHANDLE h;
  time_t * tim;
  if (seed == 0) {
    seed = time(0);
  }
  h = (MTRANDHANDLE)malloc(sizeof(struct MTRANDSTATE));
  h->mti=N+1;
  h->mt[0]= seed & 0xffffffff;
  for (h->mti=1; h->mti<N; h->mti++)
    h->mt[h->mti] = (69069 * h->mt[h->mti-1]) & 0xffffffff;
  return h;
}

#ifdef ANSIC
unsigned long genrand(MTRANDHANDLE h)
#else
unsigned long genrand(h)
MTRANDHANDLE h;
#endif
{
    unsigned long y;
    static unsigned long mag01[2]={0x0, MATRIX_A};
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    if (h->mti >= N) { /* generate N words at one time */
        int kk;

        if (h->mti == N+1)   /* if sgenrand() has not been called, */
            sgenrand(0); /* a default initial seed is used   */

        for (kk=0;kk<N-M;kk++) {
            y = (h->mt[kk]&UPPER_MASK)|(h->mt[kk+1]&LOWER_MASK);
            h->mt[kk] = h->mt[kk+M] ^ (y >> 1) ^ mag01[y & 0x1];
        }
        for (;kk<N-1;kk++) {
            y = (h->mt[kk]&UPPER_MASK)|(h->mt[kk+1]&LOWER_MASK);
            h->mt[kk] = h->mt[kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1];
        }
        y = (h->mt[N-1]&UPPER_MASK)|(h->mt[0]&LOWER_MASK);
        h->mt[N-1] = h->mt[M-1] ^ (y >> 1) ^ mag01[y & 0x1];

        h->mti = 0;
    }
  
    y = h->mt[h->mti++];
    y ^= TEMPERING_SHIFT_U(y);
    y ^= TEMPERING_SHIFT_S(y) & TEMPERING_MASK_B;
    y ^= TEMPERING_SHIFT_T(y) & TEMPERING_MASK_C;
    y ^= TEMPERING_SHIFT_L(y);

    return y; 
}
#ifdef ANSIC
double genrandr(MTRANDHANDLE h)
#else
double genrandr(h) MTRANDHANDLE h; 
#endif
{
  return ( (double)genrand(h) * 2.3283064370807974e-10 ); 
}
#ifdef ANSIC
double genrandr2(MTRANDHANDLE h) 
#else
double genrandr2(h) MTRANDHANDLE h; 
#endif
{
  return ( (double)genrand(h) *  2.3283064365386963e-10 ); 
}
#ifdef ANSIC
void destroyrand(MTRANDHANDLE h) 
#else
void destroyrand(h) MTRANDHANDLE h;
#endif
{
  free(h);
}

#ifdef ANSIC
void mt_write_randomstate(MTRANDHANDLE h,FILE* f)
#else
void mt_write_randomstate(h,f) MTRANDHANDLE h; FILE* f;
#endif
{
  int r,j;
  if (f == 0) {
    r=printf("%u\n",h->mti);
    assert(r!=EOF);
    for (j=0; j<N; j++) {
      r=printf("%u\n",h->mt[j]);
      assert(r!=EOF);
    }
  } else {
    r=fprintf(f,"%u\n",h->mti);
    assert(r!=EOF);
    for (j=0; j<N; j++) {
      r=fprintf(f,"%u\n",h->mt[j]);
      assert(r!=EOF);
    }
  }
}

#ifdef ANSIC
void mt_read_randomstate(MTRANDHANDLE h,FILE* f)
#else
void mt_read_randomstate(h,f) MTRANDHANDLE h; FILE* f;
#endif
{
  int r,j;
  if (f==0) {
    r=scanf("%u\n",&(h->mti));
    assert(r=1);
    for (j=0; j<N; j++) {
      r=scanf("%u\n",&(h->mt[j]));
      assert(r=1);
    }
  } else {
    r=fscanf(f,"%u\n",&(h->mti));
    assert(r=1);
    for (j=0; j<N; j++) {
      r=fscanf(f,"%u\n",&(h->mt[j]));
      assert(r=1);
    }
  }
}
