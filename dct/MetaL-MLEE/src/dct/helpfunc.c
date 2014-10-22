/*
 * $Source /user1/grimmer/clem/src/achim/src/RCS/helpfunc.c,v $
 * $Author: johann $
 * $Date: 2002/06/30 18:02:16 $
 * $Revision: 1.1.1.1 $
 */
/*
 *  $Log: helpfunc.c,v $
 *  Revision 1.1.1.1  2002/06/30 18:02:16  johann
 *  Initial CVS version
 *
 *  Revision 4.1  1998/07/17 11:53:20  kleiner
 *  switching to revision 4.1
 *
 * Revision 3.5  1998/07/07  16:44:30  spuhl
 * changed sort-functions to fix qsort warning
 * changed include times.h to fix warnings
 *
 * Revision 3.4  1998/07/07  06:37:16  kleiner
 * changed get_time
 * removed sys/time.h
 *
 * Revision 3.3  1998/07/02  17:03:31  spuhl
 * fixed german output
 *
 * Revision 3.2  1998/07/02  08:24:26  spuhl
 * *** empty log message ***
 *
 * Revision 3.1  1998/06/23  12:41:33  kleiner
 * *** empty log message ***
 *
 * Revision 2.5  1998/06/02  16:29:44  kleiner
 * *** empty log message ***
 *
 * Revision 2.4  1998/05/18  07:04:01  kleiner
 * *** empty log message ***
 *
 * Revision 2.3  1998/05/17  17:39:14  Administrator
 * *** empty log message ***
 *
 *  Revision 2.2  1998/05/15 14:23:24  kleiner
 *  *** empty log message ***
 *
 * Revision 2.1  1998/05/15  07:13:38  kleiner
 * *** empty log message ***
 *
 * Revision 1.4  1998/05/13  13:45:37  kleiner
 * version 2.2
 *
 * Revision 1.3  1998/05/12  14:57:51  kleiner
 * added output
 *
 * Revision 1.2  1998/05/07  16:19:02  kleiner
 * *** empty log message ***
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "framework.h"
#include "measures.h"
#include "set_var.h"
#include <time.h>  /* TK: there was no local time.h ?! */
                   /* US time.h without sys/ should work */
extern double bhep_beta;

/****************************************************************
The following method makes the distinction between numeric and
symbolic attributes. It computes from the dataset ds, the two 
datasets num and sym for numeric and symbolic attributes

Input:
ds: structur which contains the whole dataset
Output:
void
num: contains the structur for numeric attributes
sym: contains the structur for symbolic attributes
****************************************************************/

void 
NumSym (Dataset *ds, DomainInfo *names, Dataset *num, Dataset *sym) { 

  DomainInfo *res; 
  DomainInfo domain_num, domain_sym;
  AttrVal ***inst;
  int nr_num, nr_sym;
  
  res = (DomainInfo *) calloc (2, sizeof(DomainInfo));  
  res = make_domain(names, NrAttrs(names));
    
  domain_num = res[0];
  domain_sym = res[1];
     
  num -> domain = &domain_num;
  sym -> domain = &domain_sym; 
    
  num -> nr_exs = ds -> nr_exs;
  sym -> nr_exs = ds -> nr_exs;        
      
  nr_num = (num -> domain) -> nr_attrs;
  nr_sym = (sym -> domain) -> nr_attrs;          
    
  inst = (AttrVal ***) calloc (2, sizeof(AttrVal **));
  inst = get_instances(ds, nr_num, nr_sym);
  num -> instances = inst[0];
  sym -> instances = inst[1];

}

/****************************************************************
  The following method reads the values of the numeric attributes
 and stores them in a structure

Input:
ds: structur which contains the whole dataset
Output:
NumericInst: structur which contains only the values of the numeric
attributes
****************************************************************/
 
NumericInst *
read_numeric(Dataset *ds) { 
 
  NumericInst *result = NULL; 
  long int nr_ex;
  int  i,j, k = 0, nr_attr;
  
  nr_ex = ds -> nr_exs; 
  nr_attr = NrAttrs(ds -> domain);
  result = (NumericInst *) malloc (sizeof(NumericInst));
  result -> inst = matrix_mem(nr_ex, nr_attr); 
  result -> nr_ex = (int *) calloc (nr_attr, sizeof(int));
  
  for (i = 1; i <= nr_attr-1; ++i) { 
    for (j = 1; j <= nr_ex; ++j)  { 
      if(NormalVal(ds -> instances[j][i])) {   
	switch(TypeAttr(ds -> domain,i)) { 
	case integer   : result -> inst[k++][i-1] = DValAttEx(ds -> instances[j],i); 
	  break; 
	case continuous: result -> inst[k++][i-1] = CValAttEx(ds -> instances[j],i); 
	  break;            
	default:		/* (TK): added to avoid warning */
	  break;
	} 
      }
    }
    result -> nr_ex[i-1] = k;
    k = 0;
  }
  
  return result;
} 


/****************************************************************
  The following method reads the values of the numeric attributes
  and stores them in a structure

Input:
ds: structur which contains the whole dataset
Output:
NumericInst: structur which contains only the values of the numeric
attributes
****************************************************************/


 
double **
read_numeric_old(Dataset *ds) { 
 
  double **matrix; 
  long int nr_ex; 
  double value;
  int  i,j, nr_attr;
  
  nr_ex = ds -> nr_exs; 
  nr_attr = NrAttrs(ds -> domain);
  matrix = matrix_mem(nr_ex, nr_attr);
  
  for (i = 1; i <= nr_attr; ++i)  
    for (j = 1; j <= nr_ex; ++j)  {     
      switch(TypeAttr(ds -> domain,i)) { 
      case integer   : value = DValAttEx(ds -> instances[j],i); 
	break; 
      case continuous: value = CValAttEx(ds -> instances[j],i); 
	break;            
      default:	/* (TK): added to avoid warning */
	break;
      } 
      
      matrix[j-1][i-1] = value; 
      
    }
  
  return matrix;
} 
 
/****************************************************************
The following method reads the values of the symbolic attributes
 and stores them in a structure

Input:
ds: structur which contains the whole dataset
Output:
NumericInst: structur which contains only the values of the symbolic
attributes
****************************************************************/


 
double **
read_symbolic(Dataset *ds) { 
 
  double **matrix; 
  long int nr_ex; 
  double value = 0;
  int  i,j, nr_attr;
  
  nr_ex = ds -> nr_exs; 
  nr_attr = NrAttrs(ds -> domain);
  matrix = matrix_mem(nr_ex, nr_attr);
  
  for (i = 1; i <= nr_attr; ++i)  {
    for (j = 1; j <= nr_ex; ++j)  {    
      switch(TypeAttr(ds -> domain,i)) { 
      case ordered: 
      case nominal: 
	value = DValAttEx(ds -> instances[j],i); 
	break; 
      default:	/* (TK): added to avoid warning */
	break;
      } 
      matrix[j-1][i-1] = value; 
    }
  }
  return matrix;
} 




int compare_double (const void *i, const void *j) {
  
//  return (int) ( * (double *) i - * (double *) j); 
//  W.Kwedlo The above gives incorrect results for small differences e.g. (int) (0.05-0.001)==0 
    double a=*(double *)i, b=*(double *)j;
    
    if (a>b) return 1;
    if (a<b) return -1;
    return 0;
    
}

int compare_chars(const void *s1, const void *s2)
{
  return atoi((char *) s1) - atoi((char *) s2);
}

int
/* compare_attrval (AttrVal *i, AttrVal *j) { */
compare_attrval (const void *i_in, const void *j_in) {
  AttrVal *i, *j;
  
  i = (AttrVal *) i_in;
  j = (AttrVal *) j_in;

  if (i->type_of_val == j->type_of_val) {
    switch (i->type_of_val) {
    case continuous:
      return i->val.c - j->val.c;
    default:
      return i->val.d - j->val.d;
    }
  }
  return 0;
}

/****************************************************************
The followoing method sorts a given array with quick-sort

Input:
vector: array with index [low..high]
Output:
void
displays the sorted vector
****************************************************************/
 
/* void  */
/* sort_vector(double *vector, int low, int high) {  */
 
/*   int nr,i;  */
/*   int last;   */
  
/*   if (low >= high)  */
/*     return;  */

   /* TK: */ 
/*   if (low != 0) { */
/*     fprintf (stderr, "+++ sort_vector: low: %d high: %d last: %d",  */
/* 	     low, high, low + high + 1); */
/*     last = low + high + 1; */
/*   } */
/*   else { */
/*     last = high + 1; */
/*   } */
/*   qsort (vector, last, sizeof (double), compare_double); */
/*   fprintf (stderr, "+++ hinter qsort (last: %d) \n", last); */

/*   return; */
   /* TK (END) */ 

   /* The rest will no longer be executed!!! (TK) */ 
  /* Reason: qsort is a lot faster!!! */ 

/*   nr = (low + high)/2;               	 pivot-element   */  
/*   last = low;	  */
 
/*   swap (vector, nr, low);                              */
/*   for(i = low + 1; i <= high; i++)  {  */
/*     if (vector[i] < vector[low])  */
/*       swap(vector, ++last, i);  */
/*   }  */
/*   swap(vector, low, last);  */
/*   sort_vector(vector,low,last);  */
/*   sort_vector(vector,last+1,high);  */
  
/* }  */

/****************************************************************
The following method sorts the rows of a matrix

Input: 
matrix of double - values
rows: nr of rows
cols: nr. of cols
Output:
sorted matrix
****************************************************************/

/* double **sort_matrix(double **matrix, int rows, int cols) { */

/* int i,j; */
/* double **s_matrix; */
/* double *temp; */ /* (TK): unused: *vector, */ 

/*    s_matrix = matrix_mem(rows,cols); */
/*    temp = vector_mem(rows); */
    
/*    for (i = 0; i < cols; i++) { */
/*      for (j = 0; j < rows; j++)  */
/*        temp[j] = matrix[j][i]; */
/*      sort_vector(temp,0,rows-1); */
/*      for (j = 0; j < rows ; j++)          */
/*        s_matrix[j][i] = temp[j];   */
/*     } */
  
/*    free(temp); */
   
/*   return s_matrix;   */
/* } */
 
/****************************************************************/ 
 
void swap (double *vector, int i, int j) { 
 
 double temp; 
 
  temp     = vector[i]; 
  vector[i] = vector[j]; 
  vector[j] = temp; 
 
}  
 
/****************************************************************/ 
 
void 
show_matrix (double **matrix, int rows, int cols) {

  int i,j;

  for (i = 0; i < rows; i++) {  
    fprintf(stdout, "\n"); 
    for (j = 0; j < cols; j++) {
      fprintf(stdout, "mat[%d][%d] = %.8f\t", i, j, matrix[i][j]);
    }
    fflush (stdout);
  } 
}
 
 /****************************************************************/ 

void 
show_vector (double *vector, int low, int high) {

  int i;

  fprintf(stdout, "\n");
  for (i = low; i <= high; i++) {
    fprintf(stdout, "Attr. Nr. %d = %.5f\n",i+1, vector[i]);
  }
  fflush (stdout);
}

/****************************************************************/
 
double **
move_index(double **matrix, int rows, int cols) {

  int i,j;
  double **index;
  
  index = matrix_mem(rows, cols);
  
  for (i = 0; i < rows; i++) 
    for (j = 0; j < cols; j++) 
      index[i][j] = matrix[i+1][j+1];   
  
  return index;      
  
}
  
/****************************************************************/ 

void 
ShowVals() {

 fprintf (stdout, "\n#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
 fprintf (stdout, "### The following values have been used ###\n");
 fprintf (stdout, "###       to compute the measures       ###\n\n");
 fprintf (stdout, "#* BHEP-Test *\n");
 fprintf (stdout, "#\tBHEP_ALPHA = %1.2f\n",BHEP_ALPHA); 
 fprintf (stdout, "#\tBHEP_BETA  = %1.2f\n", bhep_beta);           
 fprintf (stdout, "\n#* Quartile *\n");
 fprintf (stdout, "#\tLOW_QUANT  = %1.2f\n", LOW_QUANT);  
 fprintf (stdout, "#\tMED_QUANT  = %1.2f\n", MED_QUANT);           
 fprintf (stdout, "#\tHIGH_QUANT = %1.2f\n", HIGH_QUANT);          
 fprintf (stdout, "#\n#* Alpha-trimmed Mean *\n");
 fprintf (stdout, "#\tTRIM_ALPHA = %1.2f\n", GETRIM_ALPHA);  
 fprintf (stdout, "\n#* g-Function *\n");
 fprintf (stdout, "#\tCONST = %1.2f\n", CONST);             
 fprintf (stdout, "\n#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n\n");
 fflush (stdout);
}

/***************************************************************/

char 
categ_of_examples(int nr_ex) {

  int i;
  
  for(i = 0; i < NR_CATEG_EX; i++) {
    if (nr_ex < nr_of_examples[i])
      return nr_of_categories[i];
  }
  
  return 'G';
  
}

/***************************************************************/

double **
inverse(double **matrix, int dim) {

  double **inv, **temp; 
  int i,j,k,l; 
  double value, val;
  double *hp1, *hp2;
  
  hp1 = vector_mem(dim);
  hp2 = vector_mem(dim);
  inv = matrix_mem(dim, dim);
  temp = matrix_mem(dim, dim);
  
  
  for (i = 0; i < dim; i++)  
    for (j = 0; j < dim; j++) {
      temp[i][j] = matrix[i][j]; 
      if (i == j)
	inv[i][j] = 1;
    }
  
  for (i = 0; i < dim; i++) { 
    if (temp[i][i] == 0) {
      k = i;
      while (temp[k++][i] == 0) { 
	if (k >= dim) { /* added to avoid segmentation fault */
	  fprintf (stderr, "Attention Determinant should have been 0\n");
	  break;
	}
      } 
      for (j = 0; j < dim; j++) {
	hp1[j] = temp[i][j]; 
	hp2[j] = inv[i][j];  
	temp[i][j] = temp[k-1][j];
	inv[i][j] = inv[k-1][j];
	temp[k-1][j] = hp1[j];
	inv[k-1][j] = hp2[j];
      } 
    }        
    val = temp[i][i]; 
    for (j = 0; j < dim; j++) {
      temp[i][j] *= 1.0/val; 
      inv[i][j] *= 1.0/val; 
    }
  
    for (k = 0; k < dim; k++) { 
      if (k != i) { 
	value = temp[k][i]; 
	for (l = 0; l < dim; l++) { 
	  temp[k][l] += (temp[i][l] * (-value)); 
	  inv[k][l] += (inv[i][l] * (-value)); 
	}
      }
    }
  }
  
  matrix_free(temp);
  
  return(inv); 
}

/***************************************************************/

double 
determinant(double **matrix, int dim) {

  double result,value,val; int i,j,k,l; 
  double factor = 1; 
  double **det;
  double *temp;
  int sign = 1;
  
  det = matrix_mem(dim, dim);
  temp = vector_mem(dim);

  for (i = 0; i < dim; ++i) 
   for (j = 0; j < dim; ++j) 
    det[i][j] = matrix[i][j];
 
  for (i = 0; i < dim -1 ; i++) {
    
    if ((det[i][i] < 0.0000000000001) && (det[i][i] > -0.0000000000001)) {
        k = i+1;
	/* TK: changed ABS to fabs */
        while(((fabs(det[i][k])) < 0.0000000001) && (k < dim)) { k++; }
      
        if (k == dim) {
          return 0;
        }  
        
        for (j = 0; j < dim; j++) {
            temp[j] = det[j][i];
            det[j][i] = det[j][k];
            det[j][k] = temp[j];
        }
        sign *= -1;
	   
     }
    
     
      factor *= det[i][i]; 
      val = det[i][i]; 
      for (j = 0; j < dim; j++) 
        det[i][j] *= 1.0/val; 
      for (k = i+1; k < dim; k++) { 
        value = det[k][i]; 
        for (l = i; l < dim; l++) 
          det[k][l] += (det[i][l] * (-value)); 
      }
     
  }

  result = sign * factor * det[dim-1][dim-1];


  matrix_free(det);
  vector_free(temp);

  return result;

}

/******************************************************************/ 
 
void 
get_time(char *test) {

  time_t times;  /* TK: changed back to int */
                 /* US changed back to time_t to skip warnings */
  times = time(0);
  fprintf(stderr,"\nAfter %s, %s", test, ctime(&times));
  fflush (stderr);
}

/******************************************************************/

double 
standarddev(double *vector,int nr_ex) {

  double sum_qu = 0;
  double sum_val = 0;
  int i;
  double avg, result;
  
  for(i = 0; i < nr_ex; i++) {
    sum_qu += (vector[i]*vector[i]);
    sum_val += vector[i];
  }
  
  avg = sum_val/nr_ex;
  result = sqrt((sum_qu - avg* sum_val)/nr_ex);
  
  return result;
  
}

/******************************************************************/

