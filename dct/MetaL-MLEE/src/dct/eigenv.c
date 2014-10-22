/*
 * $Source /user1/grimmer/clem/src/achim/src/RCS/eigenv.c,v $
 * $Author: johann $
 * $Date: 2002/06/30 18:02:16 $
 * $Revision: 1.1.1.1 $
 */
/*
 *  $Log: eigenv.c,v $
 *  Revision 1.1.1.1  2002/06/30 18:02:16  johann
 *  Initial CVS version
 *
 *  Revision 4.1  1998/07/17 11:53:05  kleiner
 *  switching to revision 4.1
 *
 * Revision 3.2  1998/07/02  08:24:19  spuhl
 * *** empty log message ***
 *
 * Revision 3.1  1998/06/23  12:41:28  kleiner
 * *** empty log message ***
 *
 * Revision 2.4  1998/05/18  07:03:56  kleiner
 * *** empty log message ***
 *
 * Revision 2.3  1998/05/17  17:39:06  Administrator
 * *** empty log message ***
 *
 *  Revision 2.2  1998/05/15 14:23:19  kleiner
 *  *** empty log message ***
 *
 * Revision 2.1  1998/05/15  07:13:33  kleiner
 * *** empty log message ***
 *
 * Revision 1.3  1998/05/13  13:45:25  kleiner
 * version 2.2
 *
 * Revision 1.2  1998/05/07  16:18:58  kleiner
 * *** empty log message ***
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "framework.h"
#include "measures.h"

#define SGN(x)	(x >=0)  ? 1:(-1)


/************************************************************
The following method computes for a given matrix 
the corresponding eigenvalues with QR-method 

Input: 
matrix: matrix indexed [0,dim-1][0,dim-1]
dim: dimension of the matrix
Output:
field of eigenvalues
*************************************************************/

double *
Eigenvalue(double **mat1, int dim) {

  int i,j,nr;			/* (TK): unused: k */
  double **mat_r=NULL, **mat_q=NULL, **matrix=NULL;
  double **mat_r_inv=NULL,**mat=NULL;
  double *eig_val;
  double sum = 0;   

  matrix = matrix_mem(dim,dim);
  eig_val = vector_mem(dim);
  mat=matrix_mem(dim,dim);
    
  for(i = 0; i < dim; i++)
      for(j = 0; j < dim; j++)
        mat[i][j] = mat1[i][j];


  for(nr = 1; nr <= 20; nr++) {          
    for(i = 0; i < dim; i++)
      for(j = 0; j < dim; j++)
        matrix[i][j] = mat[i][j];
   
    mat_r = QR(mat,dim);
    /*                    
       if ((ABS(determinant(mat_r,dim))) == 0) {
       printf("\nNo computation of eigenvalues");
       printf("\nDeterminant = 0, no inverse");
       exit;
       }
       */
    matrix_free(mat_r_inv);    
	mat_r_inv = inverse(mat_r,dim);
    matrix_free(mat_q);
	mat_q = MultMatQ(matrix,mat_r_inv,dim);
	mat = MultMatQ(mat_r,mat_q,dim);
	matrix_free(mat_r);
  }
  
  for(i = 0; i < dim; i++) {
    eig_val[i] = mat[i][i];
    if (eig_val[i] > 0) 
      sum += eig_val[i];
  }
  
  fprintf (stdout, "Nr_Eigenvalues: %d\n\n", dim);
  for(i = 0; i < dim; i++) {
/*    if ((eig_val[i] < 0.0000000001) && (eig_val[i] > -0.0000000001)) {}
    else {*/
    if ((eig_val[i] > 0.0000000001) || (eig_val[i] < -0.0000000001)) {
      fprintf (stdout, "Eigenvalue_%d:  %.9f\n", i+1, eig_val[i]);
      //if (eig_val[i] > 0) 
        fprintf (stdout, "Eigenvalue_Realative_%d: %f\n", i+1, eig_val[i]/sum);
    } 
        
  }

  fprintf (stdout, "\n");
  matrix_free(matrix);  
   
  return eig_val;
}

double *
Eigenvalue_old(double **mat1, int dim) {

  int i,j,nr;			/* (TK): unused: k */
  double **mat_r=NULL, **mat_q=NULL, **matrix=NULL;
  double **mat_r_inv=NULL,**mat=NULL;
  double *eig_val;
  double sum = 0;   

  matrix = matrix_mem(dim,dim);
  eig_val = vector_mem(dim);
  mat=matrix_mem(dim,dim);
    
  for(i = 0; i < dim; i++)
      for(j = 0; j < dim; j++)
        mat[i][j] = mat1[i][j];


  for(nr = 1; nr <= 20; nr++) {          
    for(i = 0; i < dim; i++)
      for(j = 0; j < dim; j++)
        matrix[i][j] = mat[i][j];
   
    mat_r = QR(mat,dim);
    /*                    
       if ((ABS(determinant(mat_r,dim))) == 0) {
       printf("\nNo computation of eigenvalues");
       printf("\nDeterminant = 0, no inverse");
       exit;
       }
       */
    matrix_free(mat_r_inv);    
	mat_r_inv = inverse(mat_r,dim);
    matrix_free(mat_q);
	mat_q = MultMatQ(matrix,mat_r_inv,dim);
	mat = MultMatQ(mat_r,mat_q,dim);
	matrix_free(mat_r);
  }
  
  for(i = 0; i < dim; i++) {
    eig_val[i] = mat[i][i];
    if (eig_val[i] > 0) 
      sum += eig_val[i];
  }
  
  //fprintf (stdout, "Nr_Eigenvalues: %d", dim);
  for(i = 0; i < dim; i++) {
/*    if ((eig_val[i] < 0.0000000001) && (eig_val[i] > -0.0000000001)) {}
    else {*/
    if ((eig_val[i] > 0.0000000001) || (eig_val[i] < -0.0000000001)) {
      fprintf (stdout, "\nEigenvalue[%d] = %.9f", i+1, eig_val[i]);
      if (eig_val[i] > 0) 
        fprintf (stdout, "\t%f", eig_val[i]/sum);
    } 
        
  }

  fprintf (stdout, "\n");
  matrix_free(matrix);  
   
  return eig_val;
}


/*************************************************************
The following method computes for a given matrix
one step of the QR-method

Input:
mat: matrix indexed [0,dim-][0,dim-1]
dim: dimension 
Output:
matrix needed for further computations
*************************************************************/

double **
QR(double **mat, int dim) {

  int i,j = 0,k;
  double **mat_t=NULL, **res;
  double norm,beta;
  double *vec,*v,*w;
  
  vec = vector_mem(dim);
  v = vector_mem(dim);
  w = vector_mem(dim);
  res = matrix_mem(dim,dim);
  
  for(i = 0; i < dim; i++) {
    norm = 0;
    for(k = i; k < dim; k++) {
      vec[k] = mat[k][i];  
      norm += quad(vec[k]);   
    }
    
    norm = sqrt(norm);
    /*
       if ((ABS(norm)) == 0) {
       printf("\nComputation of eigenvalues failed");
       continue;
       }
       */     
    for(k = i; k < dim; k++)
      v[k] = (1.0/norm) * vec[k];
      
    v[i] += SGN(vec[i]);
    /* TK: changed ABS to fabs */
    beta = (fabs(vec[i]))/(norm);
    beta = 1.0/(beta + 1);
    
    matrix_free(mat_t);
	mat_t = Transp(mat,dim);  

    for(j = i; j < dim;  j++) {
      w[j] = 0;
      for(k = i; k < dim; k++)
	w[j] += (mat_t[j][k] * v[k]); 
      w[j] *= beta; 
    }
   
    for(j = i; j < dim; j++)
      for(k = i; k < dim; k++)
        res[j][k] = v[j] * w[k]; 

    for(j = i; j < dim;  j++) 
      for(k = i; k < dim; k++) 
        mat[j][k] -= res[j][k];   
  }
  
  free(v);
  free(w);
  free(vec);
  matrix_free(res);
  matrix_free(mat_t);
  
  return mat;
}    
      
      
/*************************************************************
  The following method multiplicates two matrixes of the same
  dimension
  
  Input:
  mat1, mat2: matrixes to be multiplicate, indexed [0,dim-1][0,dim-1]
  dim: dimension of both, mat1 and mat2
  Output:
  result of multiplication is a matrix, with dimension dim, indexed [0,dim-1][0,dim-1]
**************************************************************/

double **
MultMatQ(double **mat1, double **mat2, int dim) {

  int i,j,k;
  double **mat_;
  
  
  mat_ = matrix_mem(dim,dim);

  for(i = 0; i < dim; i++) {
    for(j = 0; j < dim; j++)
      for(k = 0; k < dim; k++)
        mat_[i][j] += (mat1[i][k] * mat2[k][j]);
  }

  return mat_;
}

/************************************************************
The following method computes for a given matrix the
transputed matrix

Input:
matrix: matrix, indexed [0,dim-1][0,dim-1]
dim: dimension of the matrix
Output:
the transputed matrix, indexed [0,dim-1][0,dim-1]
************************************************************/

double **
Transp(double **matrix, int dim) {

  int i,j;
  double **tran;
  
  tran = matrix_mem(dim,dim);
  
  for(i = 0; i < dim; i++)
    for(j = 0; j < dim; j++)
      tran[i][j] = matrix[j][i];
  
  return tran;
  
} 

/***********************************************************/

