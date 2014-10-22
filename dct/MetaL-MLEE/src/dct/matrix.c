/*
 * $Source /user1/grimmer/clem/src/achim/src/RCS/matrix.c,v $
 * $Author: johann $
 * $Date: 2002/06/30 18:02:16 $
 * $Revision: 1.1.1.1 $
 */
/*
 *  $Log: matrix.c,v $
 *  Revision 1.1.1.1  2002/06/30 18:02:16  johann
 *  Initial CVS version
 *
 *  Revision 4.1  1998/07/17 11:53:36  kleiner
 *  switching to revision 4.1
 *
 * Revision 3.2  1998/07/02  08:24:34  spuhl
 * *** empty log message ***
 *
 * Revision 3.1  1998/06/23  12:41:37  kleiner
 * *** empty log message ***
 *
 * Revision 2.4  1998/06/03  14:41:01  kleiner
 * *** empty log message ***
 *
 * Revision 2.3  1998/05/18  07:04:06  kleiner
 * *** empty log message ***
 *
 * Revision 2.2  1998/05/17  17:39:20  Administrator
 * *** empty log message ***
 *
 *  Revision 2.1  1998/05/15 07:13:43  kleiner
 *  *** empty log message ***
 *
 * Revision 1.3  1998/05/13  13:45:40  kleiner
 * version 2.2
 *
 * Revision 1.2  1998/05/07  16:19:07  kleiner
 * *** empty log message ***
 *
 */

/*********************************************************************/
/*        FRAME WORK FOR PROPOSITIONAL LEARNING                      */
/*        Versao 1.0 (10/12/1996)     FILE: matrix.c                 */
/*  Developed by: Joao Gama, Luis Torgo                              */
/*                LIACC - Uni.do Porto                               */
/*                {jgama, ltorgo}@ncc.up.pt                          */
/*********************************************************************/
/* **********************************************/
/*		MATRICES			*/
/*  Some of this functions comes from the book  */
/*  NUMERICAL RECIPES IN C (2. Edition)         */
/************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "framework.h"

#define ROTATE(a,i,j,k,l) g=a[i][j]; \
h=a[k][l];a[i][j]=g-s*(h+g*tau);a[k][l]=h+s*(g-h*tau);
#define SQR(a)          ((a)*(a))
/*#define ABS(x)          ((x) > 0.0 ? (x) : -(x))*/
#define SIGN(a,b)	((b) >= 0.0 ? fabs(a) : -fabs(a))
#define SWAP(a,b)	{temp = (a); (a) = (b); (b) = temp;};
#define MAX(a,b)	((a) > (b)  ? (a) : (b))
#define MIN(a,b)	((a) > (b)  ? (b) : (a))
#define TINY	1.0e-20;
#define TRESH	1.0e-6;
#define RADIX	2
/****************************************************************/
/*		L.U Matrix decomposition 			*/
/****************************************************************/
/********************************************************/
/*	Resolucao de Equacoes 	A . X = B		*/
/*	Input	a[1..n][1..n]				*/
/*		b[1..n]					*/
/*	Output	a[1..n]					*/
/********************************************************/
int lu_solve(double **m, int n, double *b)
{
  double d;
  int status = 1, *indx = ivector(1, n);
  
  if(!ludcmp(m, n, indx, &d)) {
    status = 0;
    lubksb(m, n, indx, b);
  }
  free_ivector(indx, 1, n);
  return status;
}

int lu_msolve(double **m, int n, double **b)
{
  double d;
  int i, status = 1, *indx = ivector(1, n);
  
  if (!ludcmp(m, n, indx, &d)) {
    status = 0;
    for(i = 1; i <= n; i++)	lubksb(m, n, indx, b[i]);
  }
  free_ivector(indx, 1, n);
  return status;
}
/********************************************************/
/*	Inversao de Matrizes 				*/
/*	Input	a[1..n][1..n]		DESTROYED!	*/
/*	Output	y[1..n][1..n]				*/
/********************************************************/
double **lu_inverse(double **a, int n)
{
  double d, *col, **y = NULL;
  int i, j, *indx = ivector(1, n);
  
  if (!ludcmp(a, n, indx, &d)) {
    y = dmatrix(1, n, 1, n);
    col = dvector(1, n);
    for(j = 1; j <= n; j++) {
      for(i = 1; i <= n; i++)	col[i] = 0.0;
      col[j] = 1.0;
      lubksb(a, n, indx, col);
      for(i = 1; i <= n; i++)	y[i][j] = col[i];
    }
    free_dvector(col, 1, n);
  }
  free_ivector(indx, 1, n);
  return(y);
}
/********************************************************/
/*	Produto de Ai . B 				*/
/*	Input	a[1..n][1..n]		DESTROYED!   	*/
/*		b[1..n][1..n]		        	*/
/*	Output	y[1..n][1..n]				*/
/********************************************************/
double **lu_pmi(double **a, double **b, int n)
{
  double d, *col, **y = NULL;
  int i, j, *indx = ivector(1, n);
  
  if (!ludcmp(a, n, indx, &d)) {
    y = dmatrix(1, n, 1, n);
    col = dvector(1, n);
    for(j = 1; j <= n; j++) {
      for(i = 1; i <= n; i++)	col[i] = b[i][j];
      lubksb(a, n, indx, col);
      for(i = 1; i <= n; i++)	y[i][j] = col[i];
    }
    free_dvector(col, 1, n);
  }
  free_ivector(indx, 1, n);
  return(y);
}
/********************************************************/
/*	Determinantes	 				*/
/*	Input	a[1..n][1..n]				*/
/*	Output	d					*/
/********************************************************/
double lu_determinant(double **a, int n)
{
  double d = 0.0;
  int i, *indx = ivector(1, n);
  if (!ludcmp(a, n, indx, &d)) {
    for(i = 1; i <= n; i++)	d *= a[i][i];
  }
  free_ivector(indx, 1, n);
  /*	printf("Determinante:%6.5f\n", d);	*/
  return(d);
}
/********************************************************/
/*	Decomposicao de matrizes 	A = L . U	*/
/*	Input	a[1..n][1..n]				*/
/*	Output	a[1..n][1..n]	Tridiagonal matrix	*/
/*		indx[1..n]	permutacao utilizada	*/
/*		d		+/- 1			*/
/* Funcao utilizada para :				*/
/*	Inverter matrizes, determinantes, 		*/
/*	resolucao de sistemas				*/
/********************************************************/
int ludcmp(double **a, int n, int *indx, double *d)
{
  int i, imax, j, k;
  double big, dum, sum, temp;
  double *vv;
  
  vv = dvector(1, n);
  *d = 1.0;
  for(i = 1; i <= n; i++) {
    big = 0.0;
    for(j = 1; j <= n; j++)
      if((temp = fabs(a[i][j])) > big)	big = temp;
    if (big == 0.0)	{
      fprintf(stderr, "E)Ludcmp: Singular Matrix\n");
      free_dvector(vv, 1, n);
      return 1;
    }
    vv[i] = 1.0 / big;
  }
  for(j = 1; j <= n; j++) {
    for(i = 1; i < j; i++) {
      sum = a[i][j];
      for(k = 1; k < i; k++)	sum -= a[i][k] * a[k][j];
      a[i][j] = sum;
    }
    big = 0.0;
    for(i = j; i <= n; i++) {
      sum = a[i][j];
      for(k = 1; k < j; k++)	sum -= a[i][k]*a[k][j];
      a[i][j] = sum;
      if ((dum = vv[i] * fabs(sum)) >= big) {
	big = dum;
	imax = i;
      }
    }
    if (j != imax) {
      for(k = 1; k <= n; k++) {
	dum = a[imax][k];
	a[imax][k] = a[j][k];
	a[j][k] = dum;
      }
      *d = -(*d);
      vv[imax] = vv[j];
    }
    indx[j] = imax;
    if(a[j][j] == 0.0)	a[j][j] = TINY;
    if (j != n) {
      dum = 1.0 / (a[j][j]);
      for(i = j+1; i <= n; i++)	a[i][j] *= dum;
    }
  }
  free_dvector(vv, 1, n);
  return 0;
}
/********************************************************/
/*	Resolucao de Equacoes 	A . X = B		*/
/*	Input	a[1..n][1..n]	LU decomposicao		*/
/*		indx		permutacao de LU	*/
/*		b[1..n]					*/
/*	Output	b[1..n]		Solucao da equacao	*/
/* Funcao utilizada para :				*/
/*	Inverter matrizes, resolucao de sistemas	*/
/********************************************************/
void lubksb(double **a, int n, int *indx, double *b)
{
  int i, ii = 0, ip, j;
  double sum;
  
  for(i = 1; i <= n; i++) {
    ip = indx[i];
    sum = b[ip];
    b[ip] = b[i];
    if (ii)		for(j=ii;j<=i-1;j++)	sum -= a[i][j]*b[j];
    else		if(sum)	ii = i;
    b[i] = sum;
  }
  for(i = n; i >= 1; i--) {
    sum = b[i];
    for(j = i+1; j <= n; j++)	sum -= a[i][j] * b[j];
    b[i] = sum / a[i][i];
  }
}
/****************************************************************/
/*		Cholesky  Matrices				*/
/*		Used for Invertion of COVARIANCE Matrices	*/
/****************************************************************/
/********************************************************/
/*	Cholesky decomposicao de Matrizes		*/
/*	A = L . Lt	A simetrica, positiva definite	*/
/* Input:  a[1..n][1..n]				*/
/* Output: a[1..n][1..n] como L no triangulo inferior	*/
/*	   p[1..n]	 diagonal de L			*/
/* Comments: 	The upper part of A is unchanged	*/
/********************************************************/
int choldc(double **a, int n, double *p)
{
  int i, j, k;
  double sum;
  for(i = 1; i <= n; i++) {
    for(j=i; j <= n; j++) {
      sum = a[i][j];
      for(k = i-1; k>=1; k--) sum -=a[i][k]*a[j][k];
      if( i == j) {
	if(sum <= 0.0) {
	  fprintf(stderr, "E)Choldc: Singular Matrix\n");
	  sum = 0.0;
	  /*					return 1;	*/
	}
	p[i]=sqrt(sum);
      }
      else
	a[j][i] = sum / p[i];
    }
  }
  return 0;
}
/********************************************************/
/*	Resolucao de Equacoes	A . x = b		*/
/*	Input: 	a[1..n][1..n]	decomposta por choldc	*/
/*	 	p[1..n]		choldc			*/
/*		b[1..n]					*/
/*	Output:	x[1..n]					*/
/********************************************************/
void cholsl(double **a, int n, double *p, double *b, double *x)
{
  int i, k;
  double sum;
  for(i = 1; i <= n; i++) {
    for(sum=b[i], k = i - 1; k >= 1; k--)	sum -= a[i][k] * x[k];
    x[i] = sum/p[i];
  }
  for(i = n; i >= 1; i--) {
    for(sum = x[i], k = i+1; k <= n; k++)	sum -= a[k][i] * x[k];
    x[i] = sum/p[i];
  }
}

double **cholinv(double **a, int n)
{
  int i, j;
  double *p = dvector(1, n), *col;
  double **y = NULL;
  
  if (!choldc(a, n, p)) {
    y = dmatrix(1, n, 1, n);
    col = dvector(1, n);
    for(j = 1; j <= n; j++) {
      for(i = 1; i<= n; i++) col[i] = 0;
      col[j] = 1.0;
      cholsl(a,n,p,col, col);
      for(i = 1; i <= n; i++)	y[i][j] = col[i];
    }
    free_dvector(col, 1, n);
  }
  free_dvector(p, 1, n);
  return(y);
}

double **cholpmi(double **a, double **b, int n)
{
  int i, j;
  double *p = dvector(1, n), *col;
  double **y = NULL;
  
  if (!choldc(a, n, p)) {
    col = dvector(1, n);
    y = dmatrix(1, n, 1, n);
    for(j = 1; j <= n; j++) {
      for(i = 1; i<= n; i++) col[i] = b[j][i];
      cholsl(a,n,p,col, col);
      for(i = 1; i <= n; i++)	y[i][j] = col[i];
    }
    free_dvector(col, 1, n);
  }
  free_dvector(p, 1, n);
  return(y);
}
/********************************************************/
/*	EigenValues					*/
/*	Input 	a[1..n][1..n]				*/
/*	Output	a[1..n][1..n]				*/
/*		d[1..n]		eigenvalues		*/
/*		v[1..n][1..n]	eigenvectors		*/
/********************************************************/
double **jacobi(double **a, int n, double *d, int *nrot)
{
  int j,iq,ip,i; 
  double tresh, theta, tau, t, sm, s, h,g,c,*b,*z, **v;
  
  b = dvector(1,n); 
  z = dvector(1,n);
  v = dmatrix(1, n, 1, n);

  balanc(a, n);
  for(ip=1; ip<=n; ip++) { 
    for(iq=1; iq<=n; iq++)	v[ip][iq]=0.0; 	
    v[ip][ip]=1.0; 
  } 
  for(ip=1; ip<=n; ip++) {
    b[ip] = d[ip] = a[ip][ip];
    z[ip] = 0.0;
  }
  *nrot=0; 
  for(i = 1; i <= 50; i++) { 
    sm=0.0; 
    for(ip = 1; ip <= n-1; ip++) { 
      for(iq = ip+1; iq <= n; iq++)
	sm += fabs(a[ip][iq]); 
    } 
    if (sm == 0.0) { 
      free_dvector(z,1,n); 
      free_dvector(b,1,n); 
      return v; 
    } 
    if(i < 4)	tresh = 0.2*sm/(SQR(n)); 
    else		tresh = 0.0; 
    for(ip = 1; ip <= n-1; ip++) { 
      for(iq = ip+1; iq <= n; iq++) { 
	g=100.0*fabs(a[ip][iq]); 
	if(i > 4 && (double) (fabs(d[ip])+g) == (double) fabs(d[ip]) &&
	   (double) (fabs(d[iq])+g) == (double) fabs(d[iq]) )
	  a[ip][iq] = 0.0; 
	else 
	  if (fabs(a[ip][iq]) > tresh) { 
	    h=d[iq]-d[ip]; 
	    if ((double)(fabs(h)+g) == (double)fabs(h))
	      t = (a[ip][iq]) / h; 
	    else { 
	      theta=0.5*h/(a[ip][iq]); 
	      t=1.0/(fabs(theta)+sqrt(1.0+SQR(theta))); 
	      if (theta < 0.0) t = -t; 
	    } 
	    c = 1.0 / sqrt(1+SQR(t)); 
	    s = t*c; 
	    tau = s/(1.0+c); 
	    h = t*a[ip][iq]; 
	    z[ip] -= h; 
	    z[iq] += h; 
	    d[ip] -= h; 
	    d[iq] += h; 
	    a[ip][iq] = 0.0; 
	    for(j=1;j<=ip-1;j++) { 
	      ROTATE(a,j,ip,j,iq) 
	    } 
	    for(j=ip+1;j<=iq-1;j++) { 
	      ROTATE(a,ip,j,j,iq) 
	    } 
	    for(j=iq+1;j<=n;j++) { 
	      ROTATE(a,ip,j,iq,j) 
	    } 
	    for(j=1;j<=n;j++) { 
	      ROTATE(v,j,ip,j,iq) 
	    } 
	    ++(*nrot); 
	  } 
      } 
    } 
    for(ip=1; ip<=n; ip++) { 
      b[ip] += z[ip]; 
      d[ip] =  b[ip]; 
      z[ip] =  0.0; 
    } 
  } 
  fprintf(stderr, "E)Jacobi: Too many iterations !\n"); 
  free_dvector(z,1,n); 
  free_dvector(b,1,n); 
  return NULL;
} 

void sort_eigen(double d[], double **v, int *perm, int n, int abs)
/* 
   Last parameter 'int abs' selects comparisons: 
   0-real values   1- absolute values
*/
{ 
  int k,j,i, prev; 
  double p;

  /* TK: changed ABS to fabs */

  for(i = 1; i <= n; i++) perm[i] = i;
  for(i = 1; i < n; i++) { 
    p = d[k=i]; 
    if (!abs) {for(j = i+1; j <= n; j++)   if (d[j] >= p) p = d[k=j];} 
    else      {for(j = i+1; j <= n; j++)   if (fabs(d[j]) >= fabs(p)) p = d[k=j];}
    if (k != i) {
      prev = perm[i];
      perm[i] = perm[k];
      perm[k] = prev;

      d[k] = d[i]; 
      d[i] = p; 
      for(j = 1; j <= n; j++) { 
	p = v[j][i]; 
	v[j][i] = v[j][k]; 
	v[j][k] = p; 
      } 
    } 
  } 
}

/* (TK): used to be static */ 
void balanc(double **a, int n)
{
  int last, j, i;
  double s, r, g, f, c, sqrdx;
  
  sqrdx = SQR(RADIX);
  last = 0;
  while(last == 0) {
    last = 1;
    for(i=1;i<=n;i++) {
      r=c=0.0;
      for(j=1;j<=n;j++)
	if (j != i) {
	  c += fabs(a[j][i]);
	  r += fabs(a[i][j]);
	}
      if (c && r) {
	g = r/RADIX;
	f = 1.0;
	s = c + r;
	while(c < g) {
	  f *= RADIX;
	  c *= sqrdx;
	}
	g=r*RADIX;
	while(c>g) {
	  f /= RADIX;
	  c /= sqrdx;
	}
	if ((c+r)/f < 0.95*s) {
	  last = 0;
	  g=1.0/f;
	  for(j=1;j<=n;j++)	a[i][j] *= g;
	  for(j=1;j<=n;j++)	a[j][i] *= f;
	}
      }
    }
  }
}


