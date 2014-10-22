/*
 * $Source /user1/grimmer/clem/src/achim/src/RCS/bhep_test.c,v $
 * $Author: johann $
 * $Date: 2002/06/30 18:02:15 $
 * $Revision: 1.1.1.1 $
 */
/*
 *  $Log: bhep_test.c,v $
 *  Revision 1.1.1.1  2002/06/30 18:02:15  johann
 *  Initial CVS version
 *
 *  Revision 4.1  1998/07/17 11:52:47  kleiner
 *  switching to revision 4.1
 *
 * Revision 3.3  1998/07/02  08:24:13  spuhl
 * ..
 *
 * Revision 3.2  1998/06/29  06:28:57  spuhl
 * added results.h
 *
 * Revision 3.1  1998/06/23  12:41:24  kleiner
 * *** empty log message ***
 *
 * Revision 2.5  1998/06/02  16:29:28  kleiner
 * *** empty log message ***
 *
 * Revision 2.4  1998/05/18  07:03:53  kleiner
 * *** empty log message ***
 *
 * Revision 2.3  1998/05/17  17:38:58  Administrator
 * *** empty log message ***
 *
 *  Revision 2.2  1998/05/15 14:23:16  kleiner
 *  *** empty log message ***
 *
 * Revision 2.1  1998/05/15  07:13:31  kleiner
 * *** empty log message ***
 *
 * Revision 1.4  1998/05/13  13:45:18  kleiner
 * version 2.2
 *
 * Revision 1.3  1998/05/12  14:57:08  kleiner
 * added output (identified bhep_factor to be time consuming)
 *
 * Revision 1.2  1998/05/07  16:18:53  kleiner
 * *** empty log message ***
 *
 */

#include <stdio.h> 
#include <stdlib.h> 
#include <math.h> 
#include "framework.h" 
#include "measures.h" 
#include "estatistica.h"
#include "set_var.h"
#include "results.h"

extern int PROGINFO;

/*********************************************************************
The following method computes for a given matrix the bhep-test
value, for more details see my masterthesis
*********************************************************************/


void 
show_bhep_test(double **matrix, int rows, int cols, double **s_n, double beta) {


  double result;
  double fac1 = 0, fac2 = 0;
  double value = 0;
  double det = 0;
  
  /* TK: added det */
  det = determinant(s_n, cols);
  if (fabs (det) > 0.0000000001) {
    fac2 = 1;
    value = bhep_factor(matrix, rows, cols, s_n, beta, det);
  }
  else
    fac1 = 1; 
  
  result = rows * ((4 * fac1) + (value * fac2));
  fprintf (stdout, "\n#2. BHEP-Test\n");
  fprintf (stdout, "\tBHEPTest: %.3f\n", result);
  fflush (stdout);

  results[BHEP_RESULT] = result;
  
} 
 
 /*********************************************************************/
 
double 
bhep_factor(double **matrix, int rows, int cols, double **s_n, double beta, 
	    double det) {

/* TK:
 *
 *	added parameter 'det' == determinant(s_n,cols)
 *	otherwise the determinant is computed twice for no
 *	reason
 */

  double result, sum1, sum2;
  double temp1, temp2, temp3;
  double **inv = NULL;
  
  /* TK: changed determinant(s_n,cols) to det */
  /* TK: changed ABS to fabs */
  if (fabs(det) > 0.0000000001) {     
    inv = inverse(s_n,cols);
  }
  else {
    fprintf (stdout, "\nComputation of BHEP-Factor failed");
    fprintf (stdout, "\n\tReason: Determinant = 0\n");
    fflush (stdout);
    return 0;  /* (TK): added '0' to avoid warning */
  }
  
  sum1 = diff_norm(matrix, rows, cols, inv, beta); 
  temp1 = (1.0/quad(rows)) * sum1;

  
  sum2 =  norm(matrix, rows, cols, inv, beta);
  temp2 = - 2 * pow(1 + quad(beta), -cols/2.0) * (1.0/rows) * sum2;
  

  temp3 = pow(1 + 2 * quad(beta), -cols/2.0);
  
  result = temp1 + temp2 + temp3; 
  
  free(inv);

  return result;
  
}

/*********************************************************************/

double 
diff_norm(double **matrix, int rows, int cols, double **inv, double beta) {

  double *diff_vec = NULL;
  double sum = 0;
  double value;
  int i,j,k;
  char text[100]; /* TK */
  
  diff_vec = vector_mem(cols);
  
  for(i = 0; i < rows; i++) {
    for(j = 0; j < rows; j++) {
      for(k = 0; k < cols; k++) {
        diff_vec[k] = matrix[i][k] - matrix[j][k]; 
      }     
      value = euklid_norm(diff_vec, inv, cols);
      sum += exp(-(quad(beta)/2.0) * value);  
    }
    if (PROGINFO && ((i % 100) == 0)) { /* TK */
      sprintf (text, "%8d/%d rows [cols: %d] (BHEP_TEST)", i, rows, cols);
      get_time (text);
    }
  }

  /* TK */
  sprintf (text, "%8d/%d rows [cols: %d] (BHEP_TEST)", i, rows, cols);
  get_time (text);


  free(diff_vec);
  return sum;
 }  
   
/*********************************************************************/    
 
double norm(double **matrix, int rows, int cols, double **inv, double beta) {

int i,j,k = 0;
double *sum = NULL, *diff_vec = NULL;
double temp, value, result = 0;

  sum = vector_mem(cols);
  diff_vec = vector_mem(cols);

  for(i = 0; i < cols; i++)
    sum[i] = 0; 

  for(i = 0; i < cols; i++) {
    for(j = 0; j < rows; j++) 
       sum[i] += matrix[j][i];           
    sum[i] *= 1.0/rows;
  }
  
  temp = -quad(beta)/(2.0 * (1 + quad(beta)));
  
  for(j = 0; j < rows; j++) {
    for(i = 0; i <  cols; i++) 
       diff_vec[i] = matrix[j][i] - sum[k++];     
    value = euklid_norm(diff_vec, inv, cols);
    result += exp(temp * value); 
    k = 0;
  }
 
  free(diff_vec);
  free(sum);
  
 return result;

}

/*********************************************************************/ 
     
double euklid_norm(double *vector, double **inv, int dim) {      
     
  int i,j; /* ,k; (TK) */
  double sum = 0; 
  double *temp = NULL;
  
  temp = vector_mem(dim);   
  
  for(i = 0; i < dim; i++) 
    temp[i] = 0;
  
  for(i = 0; i < dim; i++) { 
    for(j = 0; j < dim; j++) {
      temp[i] += (vector[j] * inv[j][i]);
    }
    /* TK: added at this place to save time */
    sum += (temp[i] * vector[i]); 
  }         
  
/* TK: s. a. */
/*   for(k = 0; k < dim; k++) { */
/*     sum += (temp[k] * vector[k]);                         */
    /* temp[k] = 0; TK: not necessary */
/*  }*/
  
  free(temp);
  return sum;  
                         
}        

/*********************************************************************/    

double 
phi_func(double alpha) {

  int i;
  
  for(i = 0; i < NR_OF_VAL; i++) {
    if (alpha == alpha_phi[i][0])
      return alpha_phi[i][1];
  }
  fprintf (stdout, "\nError: alpha not in my table\n"); 
  fflush (stdout);

  return 0; /* (TK): added to avoid warning */
}

/*********************************************************************/    

double **bhep_matrix(double **p_matrix, int dim) {

double **mat = NULL;
int i,j;

 mat = matrix_mem(dim, dim);

 for(i = 0; i < dim; i++)
   for(j = 0; j < dim; j++)
      mat[i][j] = ((dim-1.0)/dim) * p_matrix[i+1][j+1];

 return mat;
}

/*********************************************************************/









