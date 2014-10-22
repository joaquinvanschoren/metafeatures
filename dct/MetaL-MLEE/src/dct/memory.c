/*
 * $Source /user1/grimmer/clem/src/achim/src/RCS/memory.c,v $
 * $Author: johann $
 * $Date: 2002/06/30 18:02:16 $
 * $Revision: 1.1.1.1 $
 */
/*
 *  $Log: memory.c,v $
 *  Revision 1.1.1.1  2002/06/30 18:02:16  johann
 *  Initial CVS version
 *
 *  Revision 4.1  1998/07/17 11:53:38  kleiner
 *  switching to revision 4.1
 *
 * Revision 3.2  1998/07/02  08:24:35  spuhl
 * *** empty log message ***
 *
 * Revision 3.1  1998/06/23  12:41:38  kleiner
 * *** empty log message ***
 *
 * Revision 2.5  1998/05/27  14:07:38  kleiner
 * *** empty log message ***
 *
 * Revision 2.4  1998/05/25  16:04:39  kleiner
 * *** empty log message ***
 *
 * Revision 2.3  1998/05/18  07:04:07  kleiner
 * *** empty log message ***
 *
 * Revision 2.2  1998/05/17  17:39:23  Administrator
 * *** empty log message ***
 *
 *  Revision 2.1  1998/05/15 07:13:44  kleiner
 *  *** empty log message ***
 *
 * Revision 1.3  1998/05/12  14:58:00  kleiner
 * *** empty log message ***
 *
 * Revision 1.2  1998/05/07  16:19:08  kleiner
 * *** empty log message ***
 *
 */

# include <stdio.h>
# include <stdlib.h>


double ** 
matrix_mem (int rows, int cols) {

  int i;
  double **matrix = NULL;

  matrix = (double **) malloc ((size_t) rows * sizeof(double *));
  if (!matrix) 
    fprintf(stderr, "Out of memory\n");     
  else {
    matrix[0] = (double *) calloc ((size_t) rows*cols, sizeof(double));
    
    for(i = 1; i < rows; i++)
      matrix[i] = matrix[i-1] + cols;
    
  }
  return matrix;
}

double *vector_mem(int dim) {
 
/* (TK): unused: int i; */
double *vector = NULL;

      vector = (double *) malloc ((size_t) dim * sizeof(double));
      if (!vector) 
         fprintf(stderr, "Out of memory\n");    
      
  return vector;
}

/****************Wojciech Kwedlo************************************/
/***************to correct some very nasty bugs*********************/

void matrix_free(double **matrix)
{
	if (matrix!=NULL)
	{
		if (matrix[0]!=NULL)
			free(matrix[0]);
		free(matrix);
	}
}

void vector_free(double *vector)
{
	if (vector!=NULL)
		free(vector);
}