/*
 * $Source /user1/grimmer/clem/src/achim/src/RCS/multi_kor.c,v $
 * $Author: johann $
 * $Date: 2002/06/30 18:02:16 $
 * $Revision: 1.1.1.1 $
 */
/*
 *  $Log: multi_kor.c,v $
 *  Revision 1.1.1.1  2002/06/30 18:02:16  johann
 *  Initial CVS version
 *
 *  Revision 4.1  1998/07/17 11:53:40  kleiner
 *  switching to revision 4.1
 *
 * Revision 3.3  1998/07/02  17:07:22  spuhl
 * fixed german output
 *
 * Revision 3.2  1998/07/02  08:24:37  spuhl
 * *** empty log message ***
 *
 * Revision 3.1  1998/06/23  12:41:39  kleiner
 * *** empty log message ***
 *
 * Revision 2.5  1998/06/02  16:29:48  kleiner
 * *** empty log message ***
 *
 * Revision 2.4  1998/05/18  07:04:09  kleiner
 * *** empty log message ***
 *
 * Revision 2.3  1998/05/17  17:39:24  Administrator
 * *** empty log message ***
 *
 *  Revision 2.2  1998/05/15 14:23:29  kleiner
 *  *** empty log message ***
 *
 * Revision 2.1  1998/05/15  07:13:45  kleiner
 * *** empty log message ***
 *
 * Revision 1.3  1998/05/14  16:40:49  kleiner
 * *** empty log message ***
 *
 * Revision 1.2  1998/05/07  16:19:09  kleiner
 * *** empty log message ***
 *
 */

# include<stdio.h>
# include<stdlib.h> 
# include <math.h> 
# include "framework.h"
# include "measures.h"


/********************************************************************
The following method computes the Coefficient of Multiple Correlation
for a given dataset

Input: 
matrix: values of numeric attributes
dim: dimension of matrix
ds: structur which contains the whole dataset
Output:
displays the value of Coefficient of Multiple Correlation
********************************************************************/
void 
show_multi_cor(double **matrix, int dim, Dataset *ds) {

  double result; 
  double **inv_matr=NULL, **temp=NULL; 
  double sum, factor; 
  int i,j,k; 
  double *vector=NULL, *vec=NULL; 
  char *name;
  
  fprintf (stdout, "\n#1. Coefficient of Multiple Correlation\n");
  fflush (stdout);
  vector = vector_mem(dim-1);
  for(k = 0; k < dim; k++) {
    
	
	matrix_free(temp);
	temp = get_mat(matrix, k, dim);
    vector_free(vec);
	vec = get_vec(matrix, k, dim);
    /*
       if (((ABS(determinant(temp,dim-1))) < 0.0000000001) || ((ABS(matrix[k][k])) < 0.0000000000001)) {
       fprintf (stdout, "\nComputation of Multipler Korrel. for field%d failed",k+1);
       printf("\nReason: Determ = 0 (no inverse), or division by 0\n");
       continue;
       } 
       */
    if (((fabs(determinant(temp,dim-1))) < 0.0000000001) || 
	((fabs(matrix[k][k])) < 0.0000000000001)) {
      fprintf (stdout, "\n#Computation of Coefficient of Multiple Correlation for field%d failed",k+1);
      printf("\n#Reason: Determ = 0 (no inverse), or division by 0\n");
      continue;
    } 
    
    matrix_free(inv_matr);
	inv_matr = inverse(temp,dim-1);

    for(i = 0; i < dim-1; i++)
      vector[i] = 0;

    for (i = 0; i < dim-1; i++) 
      for (j = 0; j < dim-1; j++) 
	vector[i] += vec[j] * inv_matr[j][i];  
 
    sum = 0;  
    for (i = 0; i < dim-1; i++) 
      sum += vector[i] * vec[i];
    /*
       if (ABS(matrix[k][k] < 0.0000000001)) {
       printf("\nComputation of Multipler Korrelationskoeff. failed");
       printf("\nTerm S_22 of the formula is zero\n");
       continue;
       } 
       */
    factor = 1.0/matrix[k][k]; 
    result = sqrt(factor*sum);

    name = ds -> domain -> attrs[k+1].name;
    fprintf (stdout, "Attr_MultiCorrel_%d: %1.3f\n", GlobalNum(ds->domain,k+1),result);  
    fflush (stdout);
  }
  vector_free(vector);
  fflush (stdout);
}

void 
show_multi_cor_old(double **matrix, int dim, Dataset *ds) {

  double result; 
  double **inv_matr=NULL, **temp=NULL; 
  double sum, factor; 
  int i,j,k; 
  double *vector=NULL, *vec=NULL; 
  char *name;
  
  fprintf (stdout, "\n#1. Coefficient of Multiple Correlation\n");
  fflush (stdout);
  vector = vector_mem(dim-1);
  for(k = 0; k < dim; k++) {
    
	
	matrix_free(temp);
	temp = get_mat(matrix, k, dim);
    vector_free(vec);
	vec = get_vec(matrix, k, dim);
    /*
       if (((ABS(determinant(temp,dim-1))) < 0.0000000001) || ((ABS(matrix[k][k])) < 0.0000000000001)) {
       fprintf (stdout, "\nComputation of Multipler Korrel. for field%d failed",k+1);
       printf("\nReason: Determ = 0 (no inverse), or division by 0\n");
       continue;
       } 
       */
    if (((fabs(determinant(temp,dim-1))) < 0.0000000001) || 
	((fabs(matrix[k][k])) < 0.0000000000001)) {
      fprintf (stdout, "\n#Computation of Coefficient of Multiple Correlation for field%d failed",k+1);
      printf("\n#Reason: Determ = 0 (no inverse), or division by 0\n");
      continue;
    } 
    
    matrix_free(inv_matr);
	inv_matr = inverse(temp,dim-1);

    for(i = 0; i < dim-1; i++)
      vector[i] = 0;

    for (i = 0; i < dim-1; i++) 
      for (j = 0; j < dim-1; j++) 
	vector[i] += vec[j] * inv_matr[j][i];  
 
    sum = 0;  
    for (i = 0; i < dim-1; i++) 
      sum += vector[i] * vec[i];
    /*
       if (ABS(matrix[k][k] < 0.0000000001)) {
       printf("\nComputation of Multipler Korrelationskoeff. failed");
       printf("\nTerm S_22 of the formula is zero\n");
       continue;
       } 
       */
    factor = 1.0/matrix[k][k]; 
    result = sqrt(factor*sum);

    name = ds -> domain -> attrs[k+1].name;
    fprintf (stdout, "\t%-10s:\t%8.3f\n", name,result);  
    fflush (stdout);
  }
  vector_free(vector);
  fflush (stdout);
}

/***************************************************************/

double **
get_mat(double **matrix, int index, int dim) {
  
  double **result;
  int i,j;
  int k = 0, l = 0;
  
  result = matrix_mem(dim-1, dim-1);
  for(i = 0; i < dim; i++) {
    if (i != index) {
      for(j = 0; j < dim; j++) {
      if (j != index)
         result[k][l++] = matrix[i][j];
      }
      k++;
      l = 0;
    }
  } 

  return result;
   
}

/***************************************************************/

double *get_vec(double **matrix, int index, int dim) {

double *result;
int i, k = 0;

   result = vector_mem(dim-1);
   for(i = 0; i < dim; i++) { 
     if(i != index)
        result[k++] = matrix[index][i]; 
   }

  return result;
}

/***************************************************************/

void print_line() {
 
  fprintf (stdout, "\n#############################################################\n");
  fflush (stdout);
  fflush (stderr);
  
}  

/***************************************************************/


