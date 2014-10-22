/*
 * $Source /user1/grimmer/clem/src/achim/src/RCS/inform.c,v $
 * $Author: johann $
 * $Date: 2002/06/30 18:02:16 $
 * $Revision: 1.1.1.1 $
 */
/*
 *  $Log: inform.c,v $
 *  Revision 1.1.1.1  2002/06/30 18:02:16  johann
 *  Initial CVS version
 *
 *  Revision 4.1  1998/07/17 11:53:22  kleiner
 *  switching to revision 4.1
 *
 * Revision 3.3  1998/07/02  17:04:54  spuhl
 * fixed german output
 *
 * Revision 3.2  1998/07/02  08:24:27  spuhl
 * *** empty log message ***
 *
 * Revision 3.1  1998/06/23  12:41:34  kleiner
 * *** empty log message ***
 *
 * Revision 2.6  1998/05/18  07:36:38  kleiner
 * *** empty log message ***
 *
 * Revision 2.5  1998/05/18  07:11:55  kleiner
 * *** empty log message ***
 *
 * Revision 2.4  1998/05/18  07:04:02  kleiner
 * *** empty log message ***
 *
 * Revision 2.3  1998/05/17  17:39:15  Administrator
 * *** empty log message ***
 *
 *  Revision 2.2  1998/05/15 14:23:25  kleiner
 *  *** empty log message ***
 *
 * Revision 2.1  1998/05/15  07:13:39  kleiner
 * *** empty log message ***
 *
 * Revision 1.2  1998/05/07  16:19:03  kleiner
 * *** empty log message ***
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "framework.h"
#include "measures.h"
#include "set_var.h"

/*************************************************************************
The following method computes the gini-index for symbolic attributes

Input: 
dist: distribution of symbolic attributes
ds: stores the whole dataset
Output:
displays the computed value of gini-index
*************************************************************************/

void 
show_gini(int ***dist, Dataset *ds) {

  int i,j, attr_index;
  double result;
  double tmp = 0, sum = 0;
  int nr_cl, nr_attr, nr_ex;
  double sum1 = 0, sum2 = 0;
  int diff_val;
  char *attribute, *target;  
  
  nr_attr = ds -> domain -> nr_attrs;
  nr_ex = ds -> nr_exs;
  nr_cl = (ds -> domain -> attrs[nr_attr]).nr_vals;
  
  for(attr_index = 1; attr_index < nr_attr; attr_index++) { 
    diff_val = (ds -> domain -> attrs[attr_index]).nr_vals;  
    for(i = 1; i <= nr_cl; i++) {
      for(j = 1; j <= diff_val; j++) 
        sum += quad(dist[attr_index][i][j]/(double) dist[nr_attr][i][i]);   
      sum *= (dist[nr_attr][i][i]/(double) nr_ex);
      tmp += sum;  
      sum = 0;
    }    
  
    result = tmp;
    tmp = 0;
    sum = 0;
  
    for(i = 1; i <= diff_val; i++) {
      for(j = 1; j <= nr_cl; j++) 
        sum += quad(dist[attr_index][j][i]/(double) dist[attr_index][0][i]);   
      sum *= (dist[attr_index][0][i]/(double) nr_ex);
      tmp += sum; 
      sum = 0;
    }   

    result += tmp;
    sum = 0;
    tmp = 0;  

    for(i = 1; i <= nr_cl; i++)
      sum1 += quad(dist[nr_attr][i][i]/(double) nr_ex);
  
    for(i = 1; i <= diff_val; i++)
      sum2 += quad(dist[attr_index][0][i]/(double) nr_ex);

    result = (result - sum1 - sum2)/(2 - sum1 - sum2);  
          
    target = ds -> domain -> attrs[nr_attr].name;
    attribute = ds -> domain -> attrs[attr_index].name;
    if(attr_index == 1) {
      fprintf (stdout, "\n#Gini-index\n");
    }
    fprintf (stdout, "Attr_Gini_sym_%d: %.3f\n",GlobalNum(ds->domain,attr_index), result);
    sum1 = 0; 
    sum2 = 0; 
    result = 0;
  }
  fflush (stdout);
} 
 
/*************************************************************************
The following method computes the relevanz-measure for symbolic attributes

Input:
dist: the distribution of symbolic attributes
ds: stores the whole dataset
Output:
displays the computed value for the relevanz-measure
*************************************************************************/


void 
show_relevanz(int ***dist, Dataset *ds) {
 
  int nr_attr, nr_ex, nr_cl;
  char *attribute, *target;
  int i,j, attr_index;
  int diff_val;
  double max = 0, sum = 0, res = 0;
  double value, result;
  
  nr_attr = ds -> domain -> nr_attrs;
  nr_ex = ds -> nr_exs;
  nr_cl = (ds -> domain -> attrs[nr_attr]).nr_vals;
  
  for(attr_index = 1; attr_index < nr_attr; attr_index++) {
    diff_val = (ds -> domain -> attrs[attr_index]).nr_vals;
    for(i = 1; i <= diff_val; i++) {
      for(j = 1; j <= nr_cl; j++) {
        value = dist[attr_index][j][i]/(double) dist[attr_index][0][i]; 
        value *= nr_ex;
        value /= dist[nr_attr][j][j];
        sum += value;
        if (value > max)
	  max = value;           
      }
      
      sum -= max;
      res += ((dist[attr_index][0][i]/(double) nr_ex) * sum);
      sum = 0;
      max = 0;       
    }

    result = 1 - (1.0/(nr_cl - 1))* res;
    target = ds -> domain -> attrs[nr_attr].name;
    attribute = ds -> domain -> attrs[attr_index].name;
    if (attr_index == 1) {
      fprintf(stdout, "\n\n#Measure of Relevance\n");
    }
    fprintf(stdout, "Attr_Relevance_%d: %.3f\n",GlobalNum(ds->domain,attr_index) , result);
    result = 0;
    res = 0;
  }
  fflush (stdout);
}       
  
/*************************************************************************
The following method computes the g-function for symbolic attribute

Input:
dist: distribution of symbolic attributes
ds: stores the whole dataset
Output:
displays the computed value for the g-function
*************************************************************************/



 
void show_g_func(int ***dist, Dataset *ds) {

int nr_attr, nr_ex, nr_cl;
int i,j, attr_index,diff_val;
double sum = 0, tmp1, tmp2;
double res = 0;		/* (TK): unused: result, */
char *attribute, *target;

  nr_attr = ds -> domain -> nr_attrs;
  nr_ex = ds -> nr_exs;
  nr_cl = (ds -> domain -> attrs[nr_attr]).nr_vals;
  
  for(attr_index = 1; attr_index < nr_attr; attr_index++) {
    diff_val = (ds -> domain -> attrs[attr_index]).nr_vals;
    for(i = 1; i <= diff_val; i++) {
      for(j = 1; j <= nr_cl; j++) 
         sum += log_fac(dist[attr_index][j][i]); 
      tmp1 = log_fac(nr_cl - 1);   
      tmp2 = log_fac(dist[attr_index][0][i] + nr_cl - 1); 
      res += (sum + tmp1 - tmp2);
      sum = 0; 
    }  
     
    target = ds -> domain -> attrs[nr_attr].name;
    attribute = ds -> domain -> attrs[attr_index].name;
    res += LOG2(CONST);
    res *= 1.0/nr_ex;  
    if (attr_index == 1)
      fprintf (stdout, "\n\n#g-Function (normalised)\n");
    fprintf (stdout, "\nAttr_gFunction_%d: %.3f", GlobalNum(ds->domain,attr_index), res);
    res = 0;
  }
  fprintf (stdout, "\n");
}        
                
/*************************************************************************/       
void 
show_g_func_old(int ***dist, Dataset *ds) {

  int nr_attr, nr_ex, nr_cl;
  int i,j, attr_index,diff_val;
  double prod1 = 1;
  double prod2 = 1;
  double factor, result, res_norm;
  char *attribute, *target;
  
  nr_attr = ds -> domain -> nr_attrs;
  nr_ex = ds -> nr_exs;
  nr_cl = (ds -> domain -> attrs[nr_attr]).nr_vals;
  
  for(attr_index = 1; attr_index < nr_attr; attr_index++) {
    diff_val = (ds -> domain -> attrs[attr_index]).nr_vals;
    for(i = 1; i <= diff_val; i++) {
      for(j = 1; j <= nr_cl; j++) 
        prod1 *= fac(dist[attr_index][j][i]); 
      factor = prod1/ fac(dist[attr_index][0][i] + nr_cl - 1); 
      prod2 *= (factor * fac(nr_cl - 1));
      prod1 = 1;
      factor = 0;
    }   
 
    target = ds -> domain -> attrs[nr_attr].name;
    attribute = ds -> domain -> attrs[attr_index].name;
    result = CONST * prod2;
    res_norm = LOG2(result)/(double) nr_ex;
    if (attr_index == 1)
      fprintf (stdout, "\n\ng-Function\n");
    fprintf (stdout, "\ng(%s, %s) = %.3f; normalised: %.3f", target, attribute, result, res_norm);
  }
  fflush (stdout);
}        

/*************************************************************************
The following method computes the value of n!

Input:
n: integer
Ouput:
n!
*************************************************************************/


unsigned long int fac(int n) {

unsigned long int result;

   if (n > 1)
    result =  n * fac(n-1);  
   else 
    result = 1; 
  return result;
}  

/*************************************************************************
The following method computes log(n!)

Input:
n: integer
Ouput:
log(n!)
*************************************************************************/

double log_fac(int n) {

int i;
double result = 0;

  for(i = 1; i <= n; i++) 
    result += LOG2(i);
     
 return result;
  
}
     
/*************************************************************************/      
      
       
     
 
 
