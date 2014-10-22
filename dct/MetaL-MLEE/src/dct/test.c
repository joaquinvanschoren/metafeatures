/*
 * $Source /user1/grimmer/clem/src/achim/src/RCS/test.c,v $
 * $Author: johann $
 * $Date: 2002/06/30 18:02:16 $
 * $Revision: 1.1.1.1 $
 */
/*
 *  $Log: test.c,v $
 *  Revision 1.1.1.1  2002/06/30 18:02:16  johann
 *  Initial CVS version
 *
 *  Revision 4.1  1998/07/17 11:54:11  kleiner
 *  switching to revision 4.1
 *
 * Revision 3.2  1998/07/02  08:25:04  spuhl
 * *** empty log message ***
 *
 * Revision 3.1  1998/06/23  12:41:56  kleiner
 * *** empty log message ***
 *
 * Revision 2.3  1998/05/18  07:04:26  kleiner
 * *** empty log message ***
 *
 * Revision 2.2  1998/05/17  17:39:34  Administrator
 * *** empty log message ***
 *
 *  Revision 2.1  1998/05/15 07:14:02  kleiner
 *  *** empty log message ***
 *
 * Revision 1.2  1998/05/14  16:48:16  kleiner
 * *** empty log message ***
 *
 */

# include <stdio.h> 
# include <stdlib.h> 
# include <math.h> 
# include "framework.h" 
# include "measures.h" 
 
# define max(a,b)  (a >= b) ? a : b; 
# define min(a,b)  (a < b)  ? a : b; 

double alpha_stddev(double *temp,int nr_ex, double ALPHA);
double std_dev(double *vector, int nr_ex);

void main() {

double *temp,result1,result2;
int i;

 temp = vector_mem(10);
 temp[0] = 2;
 temp[1] = 6.5;
 temp[2] = 6.7;
 temp[3] = 7;
 temp[4] = 10;

 result1 = alpha_stddev(temp,5,0.2);
 result2 = std_dev(temp,5);
 printf("\nResult = %.3f, %.3f, %.3f\n", result1,result2,result1/result2);
 
}


double alpha_stddev(double *temp,int nr_ex, double ALPHA) {

int k = 0,i;
double value,result;
int low, high;
double *al_getr; 

  al_getr = vector_mem(nr_ex);

  value =  ceil(nr_ex * ALPHA);
  if (value ==  (nr_ex * ALPHA)) {
     low = value + 1; 
     high = nr_ex - value; 
    }
  else {
     low = value;
     high = nr_ex - value + 1;
    }

  for(i= low-1; i <= high-1; i++) {
    al_getr[k++] = temp[i]; 
  }
   
  result = std_dev(al_getr,k);
  free(al_getr);

 return result;
} 


double std_dev(double *vector, int nr_ex) {

double sum_qu = 0, sum_val = 0;
int i;
double result,avg;

 for(i = 0; i < nr_ex; i++) {
   sum_qu += (vector[i]*vector[i]);
   sum_val += vector[i];
 }
 
 avg = sum_val/nr_ex;
 result = sqrt((sum_qu - avg* sum_val)/(nr_ex));

return result;

}

