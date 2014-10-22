/*
 * $Source /user1/grimmer/clem/src/achim/src/RCS/stat.c,v $
 * $Author: johann $
 * $Date: 2002/06/30 18:02:16 $
 * $Revision: 1.1.1.1 $
 */
/*
 * File is not needed for analist!!!
 *
 *
 *  $Log: stat.c,v $
 *  Revision 1.1.1.1  2002/06/30 18:02:16  johann
 *  Initial CVS version
 *
 *  Revision 4.1  1998/07/17 11:54:07  kleiner
 *  switching to revision 4.1
 *
 * Revision 3.2  1998/07/02  08:25:01  spuhl
 * *** empty log message ***
 *
 * Revision 3.1  1998/06/23  12:41:54  kleiner
 * *** empty log message ***
 *
 * Revision 2.5  1998/06/04  10:50:33  kleiner
 * Now mean_confidence_interval  is being used!
 *
 * Revision 2.4  1998/05/18  15:26:25  kleiner
 * THIS FILE IS NOT NEEDED BY ANALIST
 *
 * Revision 2.3  1998/05/18  07:04:23  kleiner
 * *** empty log message ***
 *
 * Revision 2.2  1998/05/17  17:39:29  Administrator
 * *** empty log message ***
 *
 *  Revision 2.1  1998/05/15 07:14:00  kleiner
 *  *** empty log message ***
 *
 * Revision 1.2  1998/05/14  16:48:14  kleiner
 * *** empty log message ***
 *
 */

/*********************************************************************/
/*        FRAME WORK FOR PROPOSITIONAL LEARNING                      */
/*        Versao 1.0 (10/12/1996)     FILE: stat.c                   */
/*  Developed by: Joao Gama, Luis Torgo                              */
/*                LIACC - Uni.do Porto                               */
/*                {jgama, ltorgo}@ncc.up.pt                          */
/*********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
/*#include "stat.h" TK: File not found!!! */
#include <float.h>

double x_tdist(unsigned int n, double sl);
double x_chi_square(unsigned int df, double alfa);
double betai(double a, double b, double x);
double gammq(double a, double x);
void gser(double *gamser, double a, double x, double *gln);
void gcf(double *gammcf, double a, double x, double *gln);
static double betacf(double a, double b, double x);

#define MAXIT	1000
#define EPS	1.0e-4
#define FPMIN	FLT_MIN


/* ======================================================================
   This is random number generator from Numerical Recipes in C.
   Returns a uniform deviate between 0.0 and 1.0 (excluding them).
   Call with idum initialized to a negative number, and then keep idum
   un-touched during a random sequence generation.
   ---------------------------------------------------------------------- */
/* float */
/* double */
/* ran1(long *idum) */
/* { */
/*   int j; */
/*   long k; */
/*   static long iy=0; */
/*   static long iv[_NTAB]; */
/*   double temp; */  /* float temp; */ 

/*   if (*idum <= 0 || !iy) { */
/*     if (-(*idum) < 1) *idum = 1; */
/*     else *idum = -(*idum); */
/*     for(j=_NTAB+7;j>=0;j--) { */
/*       k=(*idum)/_IQ; */
/*       *idum=_IA*(*idum-k*_IQ)-_IR*k; */
/*       if (*idum < 0) *idum += _IM; */
/*       if (j < _NTAB) iv[j] = *idum; */
/*     } */
/*     iy = iv[0]; */
/*   } */
/*   k = (*idum)/_IQ; */
/*   *idum = _IA*(*idum-k*_IQ)-_IR*k; */
/*   if (*idum < 0) *idum += _IM; */
/*   j = iy/_NDIV; */
/*   iy = iv[j]; */
/*   iv[j] = *idum; */
/*   if ((temp=_AM*iy) > _RNMX) return _RNMX; */
/*   else return temp; */
/* } */


/* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
   Estimation questions
   ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */

/* ======================================================================
   Obtaining a confidence interval estimation for the mean, given the estimated
   mean, the estimated standard deviation and the number of samples. Also given
   is the wanted confidence level for the estimation (a number between 0 and 1,
   i.e. a percentage).
   ---------------------------------------------------------------------- */
/* TK: Changed float to double!!! 04.06.98 */
void 
mean_confidence_interval (double M, double S, unsigned long N, double Conf, 
			  double *Mmin, double *Mmax) {

  double dist,delta;
  
  if (N > 30)               /* Use normal distribuition    */
    dist = x_tdist(1000000,Conf);
  else                   /* Use t-Student distribuition */
    dist = x_tdist(N-1,Conf);
  delta = dist * S/sqrt(N);
  *Mmin = M - delta; *Mmax = M + delta;

}


/* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
   t-Student Probability Distribuition
   ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */

/* ======================================================================
   Evaluation of the t-Student probability distribuition function.
   Returns the %area of the distribuition between -x and x for a distribuition
   with n degrees of freedom.
   ---------------------------------------------------------------------- */
double tdist(unsigned int n, double x)
{
  return 1-betai(n*0.5,0.5,n/(n+x*x));
}
 
/* ======================================================================
  Obtains the value of X that defines a certain percentage area of the
  t distribuition.
  For instance if we want to obtain the X defining a 95% interval in a
  t distribuition with 19 degrees of freedom we invoke the function as
  "x_tdist(19,0.95)"
  This would return 2.093 meaning that 95% of the t distribuition area is
  between -2.093 and 2.093.
  NOTE : If you want to obtain the values like they are presented in the
  t-student tables on statistics books you should use the macro t_table
  (that is defined with this function). t_table(N,A) correspondes to 
x_tdist(N,1-2*A)
  ---------------------------------------------------------------------- */
double x_tdist(unsigned int n, double sl)
{
  double small_x=0.0001, big_x=100, small=tdist(n,small_x),big=tdist(n,big_x);
  double midle_x, midle;
  double stop=0.0000001;

  while (sl > big || sl < small)
    if (sl > big) {
      big_x *= 2; big = tdist(n,big_x);
    } else {
      small_x /= 2; small = tdist(n,small_x);
    }

  while ((big-sl > stop) && (sl-small > stop)) {
    midle_x = small_x + (big_x-small_x)/2.0;
    midle = tdist(n,midle_x);

    if (sl > midle) {
      small = midle; small_x = midle_x;
    } else {
      big = midle; big_x = midle_x;
    }
  }

  if (sl - big < sl - small) return big_x;
  else return small_x;
}



/* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
   Chi Square Probability Distribuition
   ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */

/* ======================================================================
   The following function determines the value of Chi for which the area 
   under a chi-square distribuition with df degrees of freedom is alfa.
   This is the value that is tabled in statistics text books. More, precisely
   is the area under the curve for X greater than the value calculated (see
   the drawings on a textbook for getting a better feeling).
   ---------------------------------------------------------------------- */
double x_chi_square(unsigned int df, double alfa)
{
  double small_x=0.00001, big_x=150, 
big=gammq(df*0.5,small_x*0.5),small=gammq(df*0.5,big_x*0.5);
  double midle_x, midle;
  double stop=0.0000001;

  while (alfa > big || alfa < small)
    if (alfa > big) {
      small_x /= 2; big = gammq(df*0.5,small_x*0.5);
    } else {
      big_x *= 2; small = gammq(df*0.5,big_x*0.5);
    }

  while ((big-alfa > stop) && (alfa-small > stop)) {
    midle_x = small_x + (big_x-small_x)/2.0;
    midle = gammq(df*0.5,midle_x*0.5);

    if (alfa > midle) {
      small = midle; big_x = midle_x;
    } else {
      big = midle; small_x = midle_x;
    }
  }

  if (alfa - big < alfa - small) return big_x;
  else return small_x;
}


/* ######################################################################
   SPECIAL FUNCTIONS
   ###################################################################### */

/* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
   The Gamma Function
   ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */

/* ======================================================================
   The Gamma function (actually do to computational reasons the ln of the
   gamma function, see p.214 of Numerical Recipes).
   ---------------------------------------------------------------------- */
double gammaln(double xx)
{
  double x,y,tmp,ser;
  static double cof[6]={76.18009172947146,-86.5053203294167,24.01409824083091, 
-1.231739572450155, 0.1208650973866179e-2,-0.5395239384953e-5};
  int j;

  y=x=xx;
  tmp=x+5.5;
  tmp -= (x+0.5)*log(tmp);
  ser = 1.000000000190015;
  for(j=0;j<=5;j++) ser += cof[j]/++y;
  return -tmp+log(2.5066282746310005*ser/x);
}


/* ======================================================================
   The incomplete Gamma function [ P(a,x) ]
   ---------------------------------------------------------------------- */
double gammp(double a, double x)
{
  double gamser, gammcf,gln;

  if (x < 0.0 || a <= 0.0) printf("Invalid arguments in routine gammp (%6.6f,%6.6f)\n",a,x);
  if (x < (a+1.0)) {
    gser(&gamser,a,x,&gln);
    return gamser;
  } else {
    gcf(&gammcf,a,x,&gln);
    return 1.0 - gammcf;
  }
}


/* ======================================================================
   The incomplete Gamma function [ Q(a,x)= 1 - P(a,x) ]
   ---------------------------------------------------------------------- */
double gammq(double a, double x)
{
  double gamser, gammcf,gln;

  if (x < 0.0 || a <= 0.0) printf("Invalid arguments in routine gammq (%6.6f,%6.6f)\n",a,x);
  if (x < (a+1.0)) {
    gser(&gamser,a,x,&gln);
    return 1.0 - gamser;
  } else {
    gcf(&gammcf,a,x,&gln);
    return gammcf;
  }
}


void gser(double *gamser, double a, double x, double *gln)
{
  int n;
  double sum,del,ap;

  *gln = gammaln(a);
  if (x <= 0.0) {
    if (x < 0.0) printf("Invalid arguments in routine gser : X(%4.4f) < 0\n",x);
    *gamser=0.0;
    return;
  } else {
    ap = a;
    del = sum = 1.0/a;
    for(n=1;n<=MAXIT;n++) {
      ++ap;
      del *= x/ap;
      sum += del;
      if (fabs(del) < fabs(sum)*EPS) {
	*gamser = sum*exp(-x+a*log(x)-(*gln));
	return;
      }
    }
    printf("a(%4.4f) too large, MAXIT too small in routine gser\n",a);
    return;
  }
}


void gcf(double *gammcf, double a, double x, double *gln)
{
  int i;
  double an, b, c, d, del, h;
  
  *gln = gammaln(a);
  b = x+1.0-a;
  c = 1.0/ (double) FPMIN;
  d = 1.0/b;
  h = d;
  for(i=1;i<=MAXIT;i++) {
    an = -i*(i-a);
    b += 2.0;
    d = an*d+b;
    if (fabs(d) < FPMIN) d = FPMIN;
    c = b+an/c;
    if (fabs(c) < FPMIN) c = FPMIN;
    d = 1.0/d;
    del = d*c;
    h *= del;
    if (fabs(del-1.0) < EPS) break;
  }
  if (i > MAXIT) printf("a(%4.4f) too large, MAXIT too small in routine gcf\n",a);
  *gammcf=exp(-x+a*log(x)-(*gln))*h;
}

/* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
   The Beta Function
   ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */

/* ======================================================================
   The incomplete Beta function
   ---------------------------------------------------------------------- */
double betai(double a, double b, double x)
{
  double bt;
  if (x < 0.0 || x > 1.0)     {
    printf(" Betai: Domain Error \n");
    return 0.0;
  }
  if (x == 0.0 || x == 1.0) bt = 0.0;
  else 
    bt = exp(gammaln(a+b)- gammaln(a)-gammaln(b)+a*log(x)+b*log(1.0-x));
  if (x < (a+1.0)/(a+b+2.0)) return bt*betacf(a,b,x)/a;
  return 1.0-bt*betacf(b,a,1.0-x)/b;
}


static double betacf(double a, double b, double x)
{
  int m, m2;
  double aa, c,d,del,h,qab,qam,qap;

  qab = a + b;
  qap = a + 1.0;
  qam = a - 1.0;
  c = 1.0;
  d = 1.0 - qab * x / qap;
  if (fabs(d) < FPMIN) d = FPMIN;
  d = 1.0 / d;
  h = d;
  for(m = 1; m < MAXIT; m++) {
    m2 = 2 * m;
    aa = m * (b - m) * x / ((qam + m2) * (a + m2));
    d = 1.0 + aa * d;
    if (fabs(d) < FPMIN) d = FPMIN;
    c = 1.0 + aa / c;
    if (fabs(c) < FPMIN) c = FPMIN;
    d = 1.0 / d;
    h *= d * c;
    aa = -(a+m)*(qab+m)*x/((a+m2) * (qap + m2));
    d = 1.0 + aa * d;
    if (fabs(d) < FPMIN) d = FPMIN;
    c = 1.0 + aa / c;
    if (fabs(c) < FPMIN) c = FPMIN;
    d = 1.0 / d;
    del = d * c;
    h *= del;
    if (fabs(del - 1.0) < EPS) break;
  }
  if (m > MAXIT) {
    fprintf(stderr,"Betacf: A or B too big !");
    return -1.0;
  }
  return h;
}










