/*
 * $Source /user1/grimmer/clem/src/achim/src/RCS/quantile.c,v $
 * $Author: johann $
 * $Date: 2002/06/30 18:02:16 $
 * $Revision: 1.1.1.1 $
 */
/*
 *  $Log: quantile.c,v $
 *  Revision 1.1.1.1  2002/06/30 18:02:16  johann
 *  Initial CVS version
 *
 *  Revision 4.1  1998/07/17 11:53:42  kleiner
 *  switching to revision 4.1
 *
 * Revision 3.5  1998/07/02  17:26:45  spuhl
 * *** empty log message ***
 *
 * Revision 3.4  1998/07/02  17:07:56  spuhl
 * changed output of location and dispersion parameters
 * fixed german output
 *
 * Revision 3.3  1998/07/02  08:24:39  spuhl
 * *** empty log message ***
 *
 * Revision 3.2  1998/06/29  06:34:02  spuhl
 * saving to Measures.data removed
 *
 * Revision 3.1  1998/06/23  12:41:40  kleiner
 * *** empty log message ***
 *
 * Revision 2.7  1998/06/04  16:15:49  kleiner
 * 'isint'
 *
 * Revision 2.6  1998/05/18  07:04:10  kleiner
 * *** empty log message ***
 *
 * Revision 2.5  1998/05/17  18:22:23  Administrator
 * *** empty log message ***
 *
 *  Revision 2.4  1998/05/17 17:46:52  Administrator
 *  *** empty log message ***
 *
 *  Revision 2.3  1998/05/17 17:39:27  Administrator
 *  *** empty log message ***
 *
 *  Revision 2.2  1998/05/15 14:23:30  kleiner
 *  *** empty log message ***
 *
 * Revision 2.1  1998/05/15  07:13:46  kleiner
 * *** empty log message ***
 *
 * Revision 1.3  1998/05/13  13:45:49  kleiner
 * version 2.2
 *
 * Revision 1.2  1998/05/07  16:19:10  kleiner
 * *** empty log message ***
 *
 */

#include <stdio.h> 
#include <stdlib.h> 
#include <math.h> 
#include <string.h>
#include "framework.h" 
#include "measures.h" 
#include "set_var.h"

extern int PROGINFO;
 
/*# define max(a,b)  (a >= b) ? a : b; 
# define min(a,b)  (a < b)  ? a : b; (TK): not used */
 
/****************************************************************
The following method shows for all attributes the following measures:
Unteres Quantil, Median, Oberes Quantil, alpha-getrimmtes Mittel,
Quarilsabstand and Medianabweichung
Additional min,max and avg of alpha-getr. Mittel and min,max and avg of
Quartilsabstand are written into a file

Input: 
Dataset of numeric attributes
Output: 
void, shows the above measures
****************************************************************/ 

void 
show_lage_streu(Dataset *ds) {

  double *alphagetr, *quart, *stddev, *alphastddev;
  double *empir_low, *empir_high, *empir_med;
  double fract;	/* (TK): unused: **instances, */
  int i,j, k = 0;
  long int nr_ex;
  int nr_attr, count = 0;
  double *temp;	/* (TK): unused: , sum = 0, frac; */
  NumericInst *num_inst;
  /* (TK): unused: extern double M_SPANNW; */
  extern int NR_OUTLIERS;
  int AttrNum;
  char text[100];
  char *name;
  char out_fields[512];

  nr_attr = NrAttrs(ds -> domain);
  
  temp = vector_mem(ds-> nr_exs);
  alphagetr = vector_mem(nr_attr);
  quart = vector_mem(nr_attr);
  stddev = vector_mem(nr_attr);
  empir_low = vector_mem(nr_attr);
  empir_med = vector_mem(nr_attr);
  empir_high = vector_mem(nr_attr);
  alphastddev = vector_mem(nr_attr);  
  
  num_inst = read_numeric(ds);

  /* US */
  //fprintf (stdout, "\n#*** location and dispersion parameter ***\n\n");
  //fprintf (stdout, "%10s %10s %10s %10s %13s %12s %10s %10s\n", 
//	   "", "lower ", "", "upper ", "alpha-trimmed", "inter quar-", "deviation", "alpha stddev");
  //fprintf (stdout, "%-10s %10s %10s %10s %13s %12s %10s %10s\n", 
//	   "fieldname", "quartile", "median", "quartile", "mean   ", "tile range", "of median", "of median");
  
  for(i = 0; i < nr_attr-1; i++) { /* last attribut is target attr */
    nr_ex = num_inst -> nr_ex[i];
    for(j = 0; j < nr_ex; j++)
      temp[j] = num_inst -> inst[j][i];
    
    qsort (temp, nr_ex, sizeof (double), compare_double);

    empir_low[k] = emp_quant(temp, nr_ex, LOW_QUANT); 
    empir_med[k] = emp_quant(temp, nr_ex, MED_QUANT);
    empir_high[k] = emp_quant(temp, nr_ex, HIGH_QUANT);
    alphagetr[k] = alpha_getr(temp, nr_ex, GETRIM_ALPHA);
    quart[k] = quar_dist(temp, nr_ex, 0.25, 0.75);
    stddev[k] = med_std_dev(temp, nr_ex);
    alphastddev[k] = alpha_stddev(temp, nr_ex); 
    if (PROGINFO) {
      sprintf (text, "** %2d/%d attributes", i+1, nr_attr-1);
      get_time (text);
    }
    name = ds -> domain -> attrs[k+1].name;
	AttrNum=GlobalNum(ds->domain,k+1);	
	fprintf (stdout, "Attr_Quant025_%d: %1.3f\n",AttrNum,empir_low[k]);
	fprintf (stdout, "Attr_Quant050_%d: %1.3f\n",AttrNum,empir_med[k]);
	fprintf (stdout, "Attr_Quant075_%d: %1.3f\n",AttrNum,empir_high[k]);
	fprintf (stdout, "Attr_AlphaMean_%d: %1.3f\n",AttrNum,alphagetr[k]);
	fprintf (stdout, "Attr_InterQuartRange_%d: %1.3f\n",AttrNum,quart[k]);
	fprintf (stdout, "Attr_DevMedian_%d: %1.3f\n",AttrNum,stddev[k]);
	fprintf (stdout, "Attr_AlphaStdDevMedian_%d: %1.3f\n",AttrNum,alphastddev[k]);    
	k++;
  }

  /*   US
  fprintf (stdout, "\n*** Lageparameter ***\n");
  fprintf (stdout, "\nEmpirische Quantile\n");
  fprintf (stdout, "\nUnteres Quartil (alpha = 0.25)");
  show_values(empir_low,k,ds);
  fprintf (stdout, "\nMedian (alpha = 0.5)");
  show_values(empir_med,k,ds);
  fprintf (stdout, "\nOberes Quartil (alpha = 0.75)");
  show_values(empir_high,k,ds);
  fprintf (stdout, "\nAlpha-getrimmtes Mittel (alpha = %.2f)", GETRIM_ALPHA);
  show_values(alphagetr,k,ds);
  
  print_line();
  
  fprintf (stdout, "\n*** Streuungsparameter ***\n");
  fprintf (stdout, "\nQuartilsabstand");
  show_values(quart,k,ds); 
  fprintf (stdout, "\nMedianabweichung");
  show_values(stddev,k,ds);   
  */

  print_line();
  
  /* TK */
  strcpy(out_fields, "");
  
  for(i = 0; i < nr_attr-1; i++) {
    if (alphastddev[i] < OUT_LOW) {
      count++;
      /* TK: */
      /*      if (count > 1) fprintf (stdout, ", ");
      fprintf (stdout, "%s", ds->domain->attrs[i+1].name); US */
      /* TK */

      strcat(strcat(out_fields, " "), ds->domain->attrs[i+1].name);
    }
  }
  
  NR_OUTLIERS = count; 
  fprintf (stdout, "\n\nNumAttrsWithOutliers: %d\n",
	   NR_OUTLIERS);
  if (count) {
    fprintf(stdout, "\n# Attributes with outliers:\n\t%s\n\n", out_fields);
  }
  fflush(stdout);
  
  fract = (NR_OUTLIERS/((double)(nr_attr-1)));
  
  /* nr.of attributes with outliers    */
  /* related to the nr.of num. attr.   */

}


void 
show_lage_streu_old(Dataset *ds) {

  double *alphagetr, *quart, *stddev, *alphastddev;
  double *empir_low, *empir_high, *empir_med;
  double fract;	/* (TK): unused: **instances, */
  int i,j, k = 0;
  long int nr_ex;
  int nr_attr, count = 0;
  double *temp;	/* (TK): unused: , sum = 0, frac; */
  NumericInst *num_inst;
  /* (TK): unused: extern double M_SPANNW; */
  extern int NR_OUTLIERS;
  
  char text[100];
  char *name;
  char out_fields[512];

  nr_attr = NrAttrs(ds -> domain);
  
  temp = vector_mem(ds-> nr_exs);
  alphagetr = vector_mem(nr_attr);
  quart = vector_mem(nr_attr);
  stddev = vector_mem(nr_attr);
  empir_low = vector_mem(nr_attr);
  empir_med = vector_mem(nr_attr);
  empir_high = vector_mem(nr_attr);
  alphastddev = vector_mem(nr_attr);  
  
  num_inst = read_numeric(ds);

  /* US */
  fprintf (stdout, "\n#*** location and dispersion parameter ***\n\n");
  fprintf (stdout, "%10s %10s %10s %10s %13s %12s %10s %10s\n", 
	   "", "lower ", "", "upper ", "alpha-trimmed", "inter quar-", "deviation", "alpha stddev");
  fprintf (stdout, "%-10s %10s %10s %10s %13s %12s %10s %10s\n", 
	   "fieldname", "quartile", "median", "quartile", "mean   ", "tile range", "of median", "of median");
  
  for(i = 0; i < nr_attr-1; i++) { /* last attribut is target attr */
    nr_ex = num_inst -> nr_ex[i];
    for(j = 0; j < nr_ex; j++)
      temp[j] = num_inst -> inst[j][i];
    
    qsort (temp, nr_ex, sizeof (double), compare_double);

    empir_low[k] = emp_quant(temp, nr_ex, LOW_QUANT); 
    empir_med[k] = emp_quant(temp, nr_ex, MED_QUANT);
    empir_high[k] = emp_quant(temp, nr_ex, HIGH_QUANT);
    alphagetr[k] = alpha_getr(temp, nr_ex, GETRIM_ALPHA);
    quart[k] = quar_dist(temp, nr_ex, 0.25, 0.75);
    stddev[k] = med_std_dev(temp, nr_ex);
    alphastddev[k] = alpha_stddev(temp, nr_ex); 
    if (PROGINFO) {
      sprintf (text, "** %2d/%d attributes", i+1, nr_attr-1);
      get_time (text);
    }
    name = ds -> domain -> attrs[k+1].name;
    fprintf (stdout, "%-10.10s %10.3f %10.3f %10.3f %13.3f %12.3f %10.3f %10.3f\n", 
	     name, empir_low[k], empir_med[k], empir_high[k], alphagetr[k], 
	     quart[k], stddev[k], alphastddev[k]);
    k++;
  }

  /*   US
  fprintf (stdout, "\n*** Lageparameter ***\n");
  fprintf (stdout, "\nEmpirische Quantile\n");
  fprintf (stdout, "\nUnteres Quartil (alpha = 0.25)");
  show_values(empir_low,k,ds);
  fprintf (stdout, "\nMedian (alpha = 0.5)");
  show_values(empir_med,k,ds);
  fprintf (stdout, "\nOberes Quartil (alpha = 0.75)");
  show_values(empir_high,k,ds);
  fprintf (stdout, "\nAlpha-getrimmtes Mittel (alpha = %.2f)", GETRIM_ALPHA);
  show_values(alphagetr,k,ds);
  
  print_line();
  
  fprintf (stdout, "\n*** Streuungsparameter ***\n");
  fprintf (stdout, "\nQuartilsabstand");
  show_values(quart,k,ds); 
  fprintf (stdout, "\nMedianabweichung");
  show_values(stddev,k,ds);   
  */

  print_line();
  
  /* TK */
  strcpy(out_fields, "");
  
  for(i = 0; i < nr_attr-1; i++) {
    if (alphastddev[i] < OUT_LOW) {
      count++;
      /* TK: */
      /*      if (count > 1) fprintf (stdout, ", ");
      fprintf (stdout, "%s", ds->domain->attrs[i+1].name); US */
      /* TK */

      strcat(strcat(out_fields, " "), ds->domain->attrs[i+1].name);
    }
  }
  
  NR_OUTLIERS = count; 
  fprintf (stdout, "\n\nNr. of numeric attributes with outliers = %d\n",
	   NR_OUTLIERS);
  if (count) {
    fprintf(stdout, "\nAttributes with outliers:\n\t%s\n\n", out_fields);
  }
  fflush(stdout);
  
  fract = (NR_OUTLIERS/((double)(nr_attr-1)));
  
  /* nr.of attributes with outliers    */
  /* related to the nr.of num. attr.   */

}

/****************************************************************
The following method computes the Quartilsabstand for an attribute

Input: 
vector: contains the values of the attribute (indexed [0,nr_ex-1])
nr_ex:  number of examples for the attribute
low, high: the bounds for the computation of Quartilsabstand 
Output:
the computed value for Quartilsabstand
****************************************************************/
    
double quar_dist(double *vector, long int  nr_ex, double low, double high) { 
     
 double res1, res2;
   
   res1 = emp_quant(vector, nr_ex, low); 
   res2 = emp_quant(vector, nr_ex, high); 
  
   res2-=res1;   
   
 return res2;
}  
 
/****************************************************************
The following  method computes the Medianabweichung for an attribute

Input:
vector: contains the values of the attribute (indexed [0,nr_ex-1])
nr_exs:  number of examples for the attribute
Ouput:
the computed value for Medianabweichung
****************************************************************/
double 
med_std_dev(double *vector, long int nr_exs) {
 
  double quant;
  int i;
  double *vec; 
  double p = 0.5; 
  double val = 0, temp = 0, result = 0; 
  
  vec = vector_mem(nr_exs);
  
  quant = emp_quant(vector, nr_exs, p);  
  for (i = 0; i < nr_exs; ++i)  {
    temp = vector[i] - quant;
    vec[i] = fabs(temp); /* TK: changed ABS to fabs */
  }    

  qsort (vec, nr_exs, sizeof (double), compare_double);
  /* sort_vector(vec, 0, nr_exs-1); (TK) */
  
  val = p * nr_exs; 
  if (val == ceil(val)) 
    result = 0.5*(vec[(int)val-1] + vec[(int)val]); 
  else 
    result = vec[(int)ceil(val)-1]; 
  
  
  free(vec);
  
  return result; 
} 
 
/****************************************************************
The following  method computes the Empirisches Quantil for an attribute and
 a given bound

Input:
vector: contains the values of the attribute (indexed [0,nr_ex-1])
nr_exs:  number of examples for the attribute
p: bound
Output:
the computed value for the Empirisches Quantil
****************************************************************/
 
double emp_quant(double *vector, long int nr_exs, double p) { 
 
double v, result;
int v_int;	/* (TK): unused: ,i,j; */
 
  
  v = nr_exs * p; 
  v_int = (int) ceil(v);
  
  if (v_int == v)  
     result = 0.5*(vector[(int)v -1] + vector[(int)v]); 
  else 
     result = vector[(int)ceil(v)-1];         
    
  return result; 
}
  
/****************************************************************
The following method computes the Alpha-getrimmtes Mittel for an 
attribute and a given value for alpha 

Input:
vector: contains the values of the attribute (indexed [0,nr_ex-1])
nr_exs:  number of examples for the attribute
alpha: needed in computation
Ouput:
the computed value for the Alpha-getrimmtes Mittel
***************************************************************/
 
double alpha_getr(double *vector, long int nr_exs, double alpha) { 
 
double value;
double sum = 0; 
int low, high,i; 
double factor, result; 
  
  value =  ceil(nr_exs * alpha);
  if (value ==  (nr_exs * alpha)) {
     low = value + 1; 
     high = nr_exs - value; 
    }
  else {
     low = value;
     high = nr_exs - value + 1;
    }
             
  factor = 1.0/(nr_exs - (2 * (value-1)));
 
  for(i = low; i <= high; i++)  
    sum += vector[i-1];            
  result = factor * sum;      
    
  return result; 
 
} 
 
/****************************************************************
The following method shows the values of the given vector "values" and 
displays also the name of the corresponding attributes
ex.
attr1 - value1
attr2 - value2

Input:
values: contains for every attribute a computed value (indexed [0,nr-1])
ds: is required to get the name of the corresponding attributes
nr: nr of values stored in "values" (= nr. of attributes)
Output:
shows the values as mentioned above
*****************************************************************/

void 
show_values(double *values, int nr, Dataset *ds) {

  int i;
  char *name;

  for(i = 0; i < nr; i++) {
    name = ds -> domain -> attrs[i+1].name;
    fprintf (stdout, "\n%-10s:\t%8.3f", name,values[i]);
  }

  fprintf (stdout, "\n");
  fflush (stdout);

}    

/****************************************************************
The two methods alpha_stddev and std_dev compute the following 
measure needed for our meta-level-learning:
standard deviation(alpha-getr values for a num. attr)/
standard deviation(values for the same num. attr)
The first method computes for a given value of alpha the
 two boundaries for the alpha-getr.computation
The second one computes the above fraction
Input:
temp: contains the values of a numeric attribute 
nr_ex: nr of examples of the attribute
Output:
The above mentioned measure
****************************************************************/

double 
alpha_stddev(double *temp,int nr_ex) {

  double value,result;
  int low, high;

  value =  ceil(nr_ex * ALPHA);
  if (value ==  (nr_ex * ALPHA)) {
    low = value + 1; 
    high = nr_ex - value; 
  }
  else {
    low = value;
    high = nr_ex - value + 1;
  }
  
  result = std_dev(temp,nr_ex,low-1,high-1);
  
  return result;
} 

/****************************************************************/ 

double 
std_dev(double *vector, int nr_ex, int low, int high) {

  double sum_qu1 = 0, sum_val1 = 0;
  double sum_qu2 = 0, sum_val2 = 0;
  int i;
  double result1,avg1,result2,avg2;
  double result;
  
  for(i = 0; i < nr_ex; i++) {
    if ((i >= low) && (i <= high)) {
      sum_qu1 += (vector[i]*vector[i]);
      sum_val1 += vector[i];
    }
    sum_qu2 += (vector[i]*vector[i]);
    sum_val2 += vector[i];
  }
  
  avg1 = sum_val1/(high-low+1);
  result1 = sqrt((sum_qu1 - avg1* sum_val1)/(high-low+1));
  
  avg2 = sum_val2/nr_ex;
  result2 = sqrt((sum_qu2 - avg2* sum_val2)/nr_ex);
  
  result = result1/result2;
  
  return result;
  
}

/****************************************************************/


