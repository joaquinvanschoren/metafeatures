/*
 * $Source /user1/grimmer/clem/src/achim/src/RCS/vector.c,v $
 * $Author: johann $
 * $Date: 2002/06/30 18:02:16 $
 * $Revision: 1.1.1.1 $
 */
/*
 * Revision 5.1 2001/02/23 17:22:15 koepf
 * added ShowCovMatrix
 *
 *  $Log: vector.c,v $
 *  Revision 1.1.1.1  2002/06/30 18:02:16  johann
 *  Initial CVS version
 *
 *  Revision 4.1  1998/07/17 11:54:09  kleiner
 *  switching to revision 4.1
 *
 * Revision 3.2  1998/07/02  08:25:02  spuhl
 * *** empty log message ***
 *
 * Revision 3.1  1998/06/23  12:41:55  kleiner
 * *** empty log message ***
 *
 * Revision 2.4  1998/05/27  14:07:55  kleiner
 * *** empty log message ***
 *
 * Revision 2.3  1998/05/18  07:04:25  kleiner
 * *** empty log message ***
 *
 * Revision 2.2  1998/05/17  17:39:37  Administrator
 * *** empty log message ***
 *
 *  Revision 2.1  1998/05/15 07:14:01  kleiner
 *  *** empty log message ***
 *
 * Revision 1.2  1998/05/14  16:48:15  kleiner
 * *** empty log message ***
 *
 */

/*********************************************************************/
/*        FRAME WORK FOR PROPOSITIONAL LEARNING                      */
/*        Versao 1.0 (10/12/1996)      FILE:vector.c                 */
/*  Developed by: Joao Gama, Luis Torgo                              */
/*                LIACC - Uni.do Porto                               */
/*                {jgama, ltorgo}@ncc.up.pt                          */
/*********************************************************************/
/* ******************************************* */
/* Functions for vector and matrix             */
/* Memory Allocation                           */
/* Based on "Numerical Recipes in C"           */
/* ******************************************* */

#include <stdio.h>
#include <stdlib.h>
/* ******************************************* */
/*                Integers                     */
/* ******************************************* */
int **imatrix(long nrl,long nrh, long ncl, long nch)
/* allocate a int matrix m[nrl..nrh][ncl..nch]	*/
{
  long i, nrow = nrh - nrl+1, ncol = nch-ncl+1;
  int **m;
  
  m = (int **)  malloc((size_t) nrow * sizeof(int *));
  if (!m)	fprintf(stderr, "E)imatrix:Out of memory!\n");
  else {
    m -= nrl;
    m[nrl] = (int *) calloc((size_t) nrow*ncol, sizeof(int));
    if (!m[nrl])     fprintf(stderr, "E)imatrix: Out of memoy!\n");
    else {
      m[nrl] -= ncl;
      for(i = nrl+1; i<= nrh; i++) m[i]=m[i-1]+ncol;
    }
  }
  return m;
}

int *ivector(long nrl, long nrh)
{
  int *v;

  v = (int *) 	calloc((size_t) (nrh - nrl +1), sizeof(int));
  if (!v)		fprintf(stderr, "E)fvector: Out of memory !");
  return(v-nrl);
}

void free_ivector(int *v, long nl, long nh)
{
  free((int *) (v + nl)); 
}

void free_imatrix(int **m, long nrl, long nrh, long ncl, long nch)
{
  free((int *) (m[nrl]+ncl));
  free((int **) (m+nrl)); 
}

/* ******************************************* */
/*                Doubles                      */
/* ******************************************* */
double *dvector(long nrl, long nrh)
{
  double *v;

  v = (double *) 	calloc((size_t) (nrh - nrl +1), sizeof(double));
  if (!v)		fprintf(stderr, "E)fvector: Out of memory !");
  return(v-nrl);
}

double **dmatrix(long nrl,long nrh, long ncl, long nch)
/* allocate a double matrix m[nrl..nrh][ncl..nch]	*/
{
  long i, nrow = nrh - nrl+1, ncol = nch-ncl+1;
  double **m;
  
  m = (double **) calloc((size_t) nrow,  sizeof(double *));
  if (!m)	fprintf(stderr, "E)dmatrix:Out of memory!\n");
  else {
    m -= nrl;
    m[nrl] = (double *) calloc((size_t) nrow*ncol, sizeof(double));
    if (!m[nrl])	fprintf(stderr, "E)dmatrix: Out of memoy!\n");
    else {
      m[nrl] -= ncl;
      for(i = nrl+1; i<= nrh; i++) m[i]=m[i-1]+ncol;
    }
  }
  return m;
}

double ***dtensor(long nrm, long nrcol, long nrlin)
{
  double ***t;
  register int i;

  t = (double ***) malloc(nrm *  sizeof(double **));
  for(i = 0; i < nrm; i++)
    t[i] = dmatrix(1, nrcol, 1, nrlin);
  return t;
}

void 
free_dvector (double *v, long nl, long nh) {
  free((double *) (v + nl)); 
}

void 
free_dmatrix (double **m, long nrl, long nrh, long ncl, long nch) {
  free((double *) (m[nrl]+ncl)); 
  free((double **) (m+nrl)); 
}

void 
ShowMatrix (double **m, int n0, int n, int l0, int l) {
  register int k, j;

  for(j = n0; j <= n; j++) {
   if (j == n0) {
     fprintf (stdout, "             [%2d]  ",j);
   }
   else {
     fprintf (stdout, "      [%2d]  ", j);
   }

  } 
  fprintf (stdout, "\n");
  for(j = n0; j <= n; j++) {
    fprintf (stdout, "[%2d]   ", j);
    for(k = l0; k <= l; k++) 
      fprintf(stdout, "%10.3f  ", m[j][k]);
    fprintf (stdout, "\n");
  }
  fprintf (stdout, "\n");

  fflush (stdout);

}

void
ShowCovMatrix (double **m, int n0, int n, int l0, int l) {
  register int k, j;

  fprintf (stdout, "\n");

  for(j = n0; j <= n; j++) {
    for(k = j; k <= l; k++) {
      fprintf(stdout, "Cov_matrix_%d_%d: %-10.3f\n", j, k, m[j][k]);
    }
  }
}

void 
PrintMatrix (double **m, FILE* stream, int n0, int n, int l0, int l) {
  register int k, j;

  if (stream == NULL) {
    fprintf (stderr, "stream is NULL!\n");
  }
  
  for(j = n0; j <= n; j++) {
    if (j == n0) {
      fprintf (stream, " {{ %2d, ", n);
    }
    else {
      if (j == n0+1)
	fprintf (stream, "%2d, ", l);

      if (j != n) {
	fprintf (stream, "0, ");
      }
      else {
	fprintf (stream, "0},\n");
      }
    }
  } 

  for(j = n0; j <= n; j++) {
    fprintf (stream, "  {  0, ");
    for(k = l0; k <= l; k++) {
      fprintf (stream, "%10.3f", m[j][k]);
      if (k != l) {
	fprintf (stream, ", ");
      }
    }
    if (k != l+1) {
      fprintf (stream, "},\n");
    }
    else {
      if (j != n) 
	fprintf (stream, "}\n");
      else
	fprintf (stream, "}};\n");
    }
  }
  fprintf (stream, "\n");

  fflush (stream);
  fclose (stream);
}

/* void  */
/* ShowMatrix (double **m, int n0, int n, int l0, int l) { */
/*   register int k, j; */

/*   for(j = n0; j <= n; j++) { */
/*    if (j == n0) printf("\t\t   [%d]\t",j); */
/*    else printf("\t[%d]\t", j); */
/*   }  */
/*   printf("\n"); */
/*   for(j = n0; j <= n; j++) { */
/*     printf("[%2d]\t", j); */
/*     for(k = l0; k <= l; k++)  */
/*       printf("%9.3f\t", m[j][k]); */
/*     printf("\n"); */
/*   } */
/*   printf("\n"); */
/* } */

