/*
 * $Source /user1/grimmer/clem/src/achim/src/RCS/m_statistic.c,v $
 * $Author: johann $
 * $Date: 2002/06/30 18:02:16 $
 * $Revision: 1.1.1.1 $
 */
/*
 *  $Log: m_statistic.c,v $
 *  Revision 1.1.1.1  2002/06/30 18:02:16  johann
 *  Initial CVS version
 *
 *  Revision 4.1  1998/07/17 11:53:27  kleiner
 *  switching to revision 4.1
 *
 * Revision 3.4  1998/07/02  17:06:56  spuhl
 * fixed german output
 *
 * Revision 3.3  1998/07/02  08:24:31  spuhl
 * *** empty log message ***
 *
 * Revision 3.2  1998/06/29  06:33:10  spuhl
 * added results.h
 *
 * Revision 3.1  1998/06/23  12:41:35  kleiner
 * *** empty log message ***
 *
 * Revision 2.5  1998/06/22  12:31:37  kleiner
 * added calcualtion of chi^2-value
 *
 * Revision 2.4  1998/05/18  07:04:04  kleiner
 * *** empty log message ***
 *
 * Revision 2.3  1998/05/17  17:39:18  Administrator
 * *** empty log message ***
 *
 *  Revision 2.2  1998/05/15 14:23:27  kleiner
 *  *** empty log message ***
 *
 * Revision 2.1  1998/05/15  07:13:41  kleiner
 * *** empty log message ***
 *
 * Revision 1.2  1998/05/07  16:19:05  kleiner
 * *** empty log message ***
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "framework.h"
#include "measures.h"
#include "set_var.h"
#include "cdflib.h"
#include "results.h"

/******************************************************************
The following method computes the M-statistic for a given dataset

Input:
ds: structur which contains the whole dataset
cov: covariance matrices 
dist: distribution of symbolic attributes
Output:
displays the values for M-Statistic
******************************************************************/

void 
ShowMStat(Dataset *ds, double ***cov, int **dist) {

  int i,j,k,m;
  double **res;
  double **s;
  double det, tmp, gam;
  double result,result2, sum = 0, fac;
  int nr_ex, nr_num, nr_cl;
  double **inv, **mat;
  char *name;

  int	 which = 2;
  double p = 0.95;
  double q = 0.05;
  double chi_val = 0;
  double df = 0.0;
  int    status = 0;
  double bound = 0;
  
  nr_ex = ds -> nr_exs;
  nr_num = ds -> domain -> nr_attrs - 1;
  nr_cl = ds -> domain -> attrs[nr_num+1].nr_vals;
  
  s = matrix_mem(nr_num, nr_num);
  res = matrix_mem(nr_num, nr_num);
  inv = matrix_mem(nr_num, nr_num);
  mat = matrix_mem(nr_num, nr_num);  

  fac = (nr_ex-1)/(double)(nr_ex-nr_cl);
  for(i = 1; i <= nr_num; i++)
    for(j = 1; j <= nr_num; j++)
      s[i-1][j-1] = fac * cov[0][i][j];     /* S~ */

  printf("\n#3. M-Statistic\n");
  
  for(m = 1; m <= nr_cl; m++) {     
                         
    mat = move_index(cov[m], nr_num, nr_num);
    det = determinant(mat,nr_num);
    
    if((det <= 0.0000000001) && (det >= -0.0000000001)) {
      name = LblValId(ds -> domain,nr_num+1,m);
      fprintf(stdout, "\n#The computation of M-Statistic failed\n");
      fprintf(stdout, "#Reason: Determinant of matrix for class %s is zero\n",
	      name);
      return;
    }
    
    
    inv = inverse(mat, nr_num);
    for(i = 0; i < nr_num; i++)
      for(j = 0; j < nr_num; j++) {
        res[i][j] = 0;
        for(k = 0; k < nr_num; k++) 
          res[i][j] += (inv[i][k]*s[k][j]);  
      }     
    
    det = determinant(res, nr_num); 
    tmp = log(det);
    sum += ((dist[m][m]-1)*tmp);
    
  }
  
  m_calculated = TRUE;
  gam = gamma_(nr_num, nr_cl, dist, nr_ex);
  result = gam * sum;
  result2 = nr_num * (nr_num + 1) * (nr_cl -1) / 2;              /* NEW */
  //fprintf (stdout, "\tResult = %.3f         degrees of freedom: %.0f \n", result, result2);
 
  fprintf (stdout, "MStatistic: %.3f\nMStatDF: %.0f\n", result, result2);

  results[M_STATISTIC] = result;
  results[DEGR_FREE] =  result2;

  which = 2;
  p = CHI_GAMMA;
  q = 1-CHI_GAMMA;
  chi_val = 0;
  df = result2;
  status = 0;
  bound = 0;
  
  fflush (stdout);
  cdfchi (&which, &p, &q, &chi_val, &df, &status, &bound);
  
  fprintf (stdout, "MStatChiSq: %3.2f\nMStatChiSqAlpha: %.2f\n\n",CHI_GAMMA, chi_val);
  fprintf (stdout, "#(Result and chi^2-value should be approximately equal)\n");

  results[M_STAT_CHI] = chi_val;
  
  fflush (stdout);
  if (status != 0) {
    fprintf (stderr, "\tstatus != 0\n");
    fflush (stderr);
  }
  print_line();
  fprintf(stdout, "\n#4. Sd Ratio\n");
  result = Sd_ratio(result, nr_num, nr_cl, dist);
  fprintf(stdout, "SDRatio: %.3f\n", result);

  results[SD_RATIO] = result;
  
  fflush (stdout);
  free(inv);
  free(mat);
  free(res);
  
}

ShowMStat_old(Dataset *ds, double ***cov, int **dist) {

  int i,j,k,m;
  double **res;
  double **s;
  double det, tmp, gam;
  double result,result2, sum = 0, fac;
  int nr_ex, nr_num, nr_cl;
  double **inv, **mat;
  char *name;

  int	 which = 2;
  double p = 0.95;
  double q = 0.05;
  double chi_val = 0;
  double df = 0.0;
  int    status = 0;
  double bound = 0;
  
  nr_ex = ds -> nr_exs;
  nr_num = ds -> domain -> nr_attrs - 1;
  nr_cl = ds -> domain -> attrs[nr_num+1].nr_vals;
  
  s = matrix_mem(nr_num, nr_num);
  res = matrix_mem(nr_num, nr_num);
  inv = matrix_mem(nr_num, nr_num);
  mat = matrix_mem(nr_num, nr_num);  

  fac = (nr_ex-1)/(double)(nr_ex-nr_cl);
  for(i = 1; i <= nr_num; i++)
    for(j = 1; j <= nr_num; j++)
      s[i-1][j-1] = fac * cov[0][i][j];     /* S~ */

  printf("\n3. M-Statistic\n");
  
  for(m = 1; m <= nr_cl; m++) {     
                         
    mat = move_index(cov[m], nr_num, nr_num);
    det = determinant(mat,nr_num);
    
    if((det <= 0.0000000001) && (det >= -0.0000000001)) {
      name = LblValId(ds -> domain,nr_num+1,m);
      fprintf(stdout, "\nThe computation of M-Statistic failed\n");
      fprintf(stdout, "Reason: Determinant of matrix for class %s is zero\n",
	      name);
      return;
    }
    
    
    inv = inverse(mat, nr_num);
    for(i = 0; i < nr_num; i++)
      for(j = 0; j < nr_num; j++) {
        res[i][j] = 0;
        for(k = 0; k < nr_num; k++) 
          res[i][j] += (inv[i][k]*s[k][j]);  
      }     
    
    det = determinant(res, nr_num); 
    tmp = log(det);
    sum += ((dist[m][m]-1)*tmp);
    
  }
  
  m_calculated = TRUE;
  gam = gamma_(nr_num, nr_cl, dist, nr_ex);
  result = gam * sum;
  result2 = nr_num * (nr_num + 1) * (nr_cl -1) / 2;              /* NEW */
  fprintf (stdout, "\tResult = %.3f         degrees of freedom: %.0f \n", result, result2);

  results[M_STATISTIC] = result;
  results[DEGR_FREE] =  result2;

  which = 2;
  p = CHI_GAMMA;
  q = 1-CHI_GAMMA;
  chi_val = 0;
  df = result2;
  status = 0;
  bound = 0;
  
  fflush (stdout);
  cdfchi (&which, &p, &q, &chi_val, &df, &status, &bound);
  
  fprintf (stdout, "\tchi^2(%.0f,%3.2f) = %.2f\n\n", 
	   result2, CHI_GAMMA, chi_val);
  fprintf (stdout, "\t(Result and chi^2-value should be approximately equal)\n");

  results[M_STAT_CHI] = chi_val;
  
  fflush (stdout);
  if (status != 0) {
    fprintf (stderr, "\tstatus != 0\n");
    fflush (stderr);
  }
  print_line();
  fprintf(stdout, "\n4. Sd Ratio\n");
  result = Sd_ratio(result, nr_num, nr_cl, dist);
  fprintf(stdout, "\tResult = %.3f\n", result);

  results[SD_RATIO] = result;
  
  fflush (stdout);
  free(inv);
  free(mat);
  free(res);
  
}
/******************************************************************/

double 
gamma_(int nr_num, int nr_cl, int **dist, int nr_ex) {

  int i;
  double term1, term2;
  double sum = 0, result;
  
  term1 = (2*quad(nr_num) + 3 * nr_num -1)/(double)(6 *(nr_num + 1)*(nr_cl -1));
  for(i = 1; i <= nr_cl; i++)
    sum += 1.0/(dist[i][i]-1);
  term2 = sum - 1.0/(nr_ex-nr_cl);

  result = 1 - term1*term2;
  return result;

}          

/******************************************************************
  The following method computes the value for Sd-ratio
  
  Input:
  m: result of M-Statistic
  nr_num: nr of numeric attributes
  nr_cl: nr. of classes
  dist: distribution of symbolic attributes
  Output:
  value of sd-ratio
  *****************************************************************/

double 
Sd_ratio(double m, int nr_num, int nr_cl, int **dist) {

  int i;
  double result;
  double sum = 0;

  for(i = 1; i <= nr_cl; i++) 
    sum += (dist[i][i]-1);
  sum *= nr_num;
  result = exp(m/sum);

  return result;

}

/******************************************************************/







