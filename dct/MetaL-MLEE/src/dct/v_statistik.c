/*
 * $Source /user1/grimmer/clem/src/achim/src/RCS/v_statistik.c,v $
 * $Author: johann $
 * $Date: 2002/06/30 18:02:16 $
 * $Revision: 1.1.1.1 $
 */
/*
 *  $Log: v_statistik.c,v $
 *  Revision 1.1.1.1  2002/06/30 18:02:16  johann
 *  Initial CVS version
 *
 *  Revision 4.1  1998/07/17 11:53:53  kleiner
 *  switching to revision 4.1
 *
 * Revision 3.4  1998/07/02  17:11:02  spuhl
 * fixed german output
 *
 * Revision 3.3  1998/07/02  08:24:48  spuhl
 * *** empty log message ***
 *
 * Revision 3.2  1998/06/29  06:40:38  spuhl
 * added results.h
 *
 * Revision 3.1  1998/06/23  12:41:46  kleiner
 * *** empty log message ***
 *
 * Revision 2.5  1998/06/22  12:32:37  kleiner
 * added calculation of chi^2-value
 *
 * Revision 2.4  1998/05/18  07:04:16  kleiner
 * *** empty log message ***
 *
 * Revision 2.3  1998/05/17  17:39:36  Administrator
 * *** empty log message ***
 *
 *  Revision 2.2  1998/05/15 14:23:34  kleiner
 *  *** empty log message ***
 *
 * Revision 2.1  1998/05/15  07:13:52  kleiner
 * *** empty log message ***
 *
 * Revision 1.4  1998/05/14  16:40:57  kleiner
 * *** empty log message ***
 *
 * Revision 1.3  1998/05/13  13:45:57  kleiner
 * version 2.2
 *
 * Revision 1.2  1998/05/07  16:19:16  kleiner
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


/********************************************************************
The following method computes the V-statistic for numerich attributes

Input: 
eig_vector: array which contains the computed eigenvalues of
matrix (Within)^-1* B
ds: structur which contains the whole dataset
Output:
displays the value for V-statistic
********************************************************************/

void 
show_v_stat(double *eig_vector, Dataset *ds) {

  double res, bart,can,frac;
  int i = 0, k = 0; /* (TK): unused: j */
  double *pos_vec;
  double result; /* (TK): unused: , sum = 0.0; */
  int nr_attr, nr_cl, nr_eig; /* (TK): unused: , lim; */
  unsigned int nr_ex;
  DomainInfo *dom;
  double fac;
  int count = 0;

  int	 which = 2;
  double p = 0.95;
  double q = 0.05;
  double chi_val = 0;
  double df = 0.0;
  int    status = 0;
  double bound = 0;
  
  if(eig_vector == NULL) {
    fprintf (stdout, "\nNo computation of V-Statistic");
    fprintf (stdout, "\nReason: No eigenvalues\n");
    fflush (stdout);
    return;			/* TK (very important) used to be at the wrong place */
  }
  
  dom = ds -> domain;
  nr_cl = NValsAttr(dom, dom -> nr_attrs);
  nr_attr = ds -> domain -> nr_attrs - 1;
  nr_ex = ds -> nr_exs;
  nr_eig = nr_attr;
  pos_vec = vector_mem(nr_eig);
  
  qsort (eig_vector, nr_eig, sizeof (double), compare_double);  
  /* sort_vector(eig_vector,0, nr_eig-1); (TK) */
  
  while (i < nr_eig) {
    if(eig_vector[i] > 0.00000000001) 
      pos_vec[k++] = eig_vector[i];  
    i++;
  }
  
  if (k == 0)
    fprintf (stdout, "\nNr. of positiv Eigenvalues = 0\n");
  else {   
    v_calculated = TRUE;
    frac = Fract(pos_vec,k);   
    can = Cancor(pos_vec[k-1]);
    fprintf (stdout, "\nFract: %f", frac);
    fprintf (stdout, "\nCancor: %f\n", can);
    print_line();
   
    results[FRACT1] = frac;
    results[CANCOR1] = can;
    
    fprintf (stdout, "\n#*** V-Statistic ***\n");
    res = wilk_lambda(pos_vec,k);  
    //fprintf (stdout, "\nWilks Lambda\n");
    fprintf (stdout, "WilksLambda: %f\n", res);
    
    results[WILKS_LAMBDA] = res;
    
    bart = bartlett_stat(pos_vec,k,nr_attr, nr_cl, nr_ex);
    //fprintf (stdout, "\nBartletts Statistic\n");
    fprintf (stdout, "BartlettStatistic: %f\n", bart);
    
    results[BARTLETT] = bart;
    
    fflush (stdout);

    which = 2;
    p = CHI_GAMMA;
    q = 1-CHI_GAMMA;
    chi_val = 0;
    df = nr_attr * (nr_cl-1);
    status = 0;
    bound = 0;
    
    fflush (stdout);
    cdfchi (&which, &p, &q, &chi_val, &df, &status, &bound);
    
    fprintf (stdout, "BartlettStat_ChiSq_DF: %1.3f\n",(double) nr_attr * (nr_cl-1)); 
	fprintf (stdout, "BartlettStat_ChiSq_Alpha: %1.3f\n",CHI_GAMMA); 
	fprintf (stdout, "BartlettStat_ChiSq_Value: %1.3f\n",chi_val); 

//	fprintf (stdout, "\tchi^2(%.0f,%3.2f) = %.2f\n\n", 
//	     (double) nr_attr * (nr_cl-1), CHI_GAMMA, chi_val);
    fprintf (stdout, "#(Result should be greater than the chi^2-value)\n");

    results[V_STAT_CHI] = chi_val;
    
    fflush (stdout);
    if (status != 0) {
      fprintf (stderr, "\tstatus != 0\n");
      fflush (stderr);
    }
    

  
    if (k == 1) { 
      fprintf (stdout,"\n#Only 1 positiv eigenvalue");
      fprintf (stdout,"\n#No test of significance\n");
      fflush (stdout);
    }
    else {
      fprintf (stdout, "\n\n#*** Test auf Signifikanz ***\n\n");  
      
      for(i = 0; i < k-1; i++) { 
	fac = eigen_val(pos_vec[k-1-i], nr_attr, nr_cl, nr_ex);
	if (!i) 
	  result = bart - fac;
	else 
	  result -= fac;
	if (result >= 0.0000000001) {   
	  fprintf (stdout, "Diskriminanzfunktion_%d: %f\n",i+1,result);
	  count++;
	}
      }	
	  fprintf(stdout,"NDiscrimFunct: %d\n",count); 
      results[NR_DISC_FKTN] = count;
      
    }      
  }
} 
    
/********************************************************************
The following method computes the value for Wilk's Lambda

Input:
s_eig_vec: array of sorted (positive) eigenvalues
nr_eig: nr. of positive eigenvalues
Output:
value of Wilk's Lambda
********************************************************************/

double wilk_lambda(double *s_eig_vec, int nr_eig) {

double prod = 1, result;
int i;

  for(i = 0; i < nr_eig; i++)
    prod *= (1+ s_eig_vec[i]);   
  
  result = 1.0/prod;
 
 return result;
 
}    

/********************************************************************
The following method computes the value for Bartlett's statistic

Input:
eig_vec: array of positive eigenvalues
nr_eig: nr. of pos. eigenvalues
nr_attr: nr. of attributes
nr_cl: nr. of classes
nr_ex: nr. of instances
Output:
value for Bartlett's statistic
********************************************************************/

double bartlett_stat(double *eig_vec, int nr_eig, int nr_attr, int nr_cl, int nr_ex) {

double sum = 0, result;
int i;

  for(i = 0; i < nr_eig; i++) 
    sum +=  log(1 + eig_vec[i]);
    
  result = (nr_ex- 1 -0.5*(nr_attr+nr_cl))*sum;
  
 return result;
 
} 

/********************************************************************/

double eigen_val(double value, int nr_attr, int nr_cl, int nr_ex) {

double result;
/* (TK): unused: int i; */

  result = (nr_ex - 1 - 0.5*(nr_attr+nr_cl))*log(1+value);
 
 return result;

}   

/********************************************************************
The following method computes Fract

Input: 
eigval: array of positive eigenvalues
nr: nr of positve eigenvalues
Output:
value of Fract
********************************************************************/

double Fract(double *eigval,int nr) {

int i;
double result = 0;

   for(i = 0; i < nr; i++)
     result += eigval[i];
     
   result = eigval[nr-1]/result;
   
  return result;

}

/********************************************************************
The following method computes Cancor

Input: 
maxeig: the largest positive eigenvalue
Output:
value of Cancor
********************************************************************/
 
double Cancor(double maxeig) {

double result;

   result = sqrt(maxeig/(1+maxeig));
   
 return result;
 
}        

/********************************************************************/



