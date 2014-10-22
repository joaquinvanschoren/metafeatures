/*
 * $Source /user1/grimmer/clem/src/achim/src/RCS/crit_val.c,v $
 * $Author: johann $
 * $Date: 2002/06/30 18:02:15 $
 * $Revision: 1.1.1.1 $
 */
/*
 *  $Log: crit_val.c,v $
 *  Revision 1.1.1.1  2002/06/30 18:02:15  johann
 *  Initial CVS version
 *
 *  Revision 4.1  1998/07/17 11:53:55  kleiner
 *  switching to revision 4.1
 *
 * Revision 3.3  1998/07/02  08:24:49  spuhl
 * *** empty log message ***
 *
 * Revision 3.2  1998/06/29  06:41:08  spuhl
 * show_crit_value replaced by calc_crit_value
 *
 * Revision 3.1  1998/06/23  12:41:47  kleiner
 * *** empty log message ***
 *
 * Revision 2.5  1998/06/02  16:29:54  kleiner
 * *** empty log message ***
 *
 * Revision 2.4  1998/05/18  07:04:17  kleiner
 * *** empty log message ***
 *
 * Revision 2.3  1998/05/17  17:39:03  Administrator
 * *** empty log message ***
 *
 *  Revision 2.2  1998/05/15 14:23:35  kleiner
 *  *** empty log message ***
 *
 * Revision 2.1  1998/05/15  07:13:53  kleiner
 * *** empty log message ***
 *
 * Revision 1.2  1998/05/08  06:43:27  kleiner
 * added extern get_time...
 *
 */

# include<stdio.h>
# include<stdlib.h>
# include<math.h>
# include "framework.h"
# include "measures.h"


/********************************************************************
The following method computes a critical value, which is needed
for the bhep test
********************************************************************/

/*  US replaced by calc_crit_value 

void    
show_crit_value(double beta, double p, double value) {

  double fac1, fac2;
  double result;
  
  double m = mue (beta, p);
  double s = sigma (beta,p);

  fac1 = m * pow ((1+(quad(s)/quad(m))),-0.5);
  fac2 = exp (value * sqrt (log (1+ (s/m))));

  result = fac1 * fac2;
  
  fprintf(stdout, "\tCritical value = %1.3f\n", result);
  fflush (stdout);
  
}
 
 */

double
calc_crit_value(double beta, double p, double value) {

  double fac1, fac2;
  double result;
  
  double m = mue (beta, p);
  double s = sigma (beta,p);

  fac1 = m * pow ((1+(quad(s)/quad(m))),-0.5);
  fac2 = exp (value * sqrt (log (1+ (s/m))));

  result = fac1 * fac2;
  
  return result;
  
}


/********************************************************************/

double 
mue(double beta, double p) {

  double temp,result;
  double gama;
  
  gama = 1 + 2*quad(beta);
  /* temp = 1 + (p*quad(beta))/gama + (p*(p+2)*qquad(beta))/(2*quad(gama)); */
  temp = 1 + (p*quad(beta))/gama + (p*(p+2)*pow(beta,4))/(2*quad(gama));
  
  result = 1 - pow(gama, -(p/2.0))*temp; 

  return result;
  
}

/********************************************************************/ 

double 
sigma(double beta, double p) {

  double fac2, fac3;
  double term1, term2, term3;
  double sum2, sum3, gama;
  double delta, result;
  
  delta = 1 + 4*quad(beta) + 3*qquad(beta);
  gama = 1 + 2*quad(beta);
  
  term1 = 2*pow((1 + 4*quad(beta)),-p/2.0);
  
  fac2 = 2*pow(gama,-p);
  sum2 = (1 + 2*p*qquad(beta)/quad(gama)); 
  sum2 += (3*p*(p + 2)*qqquad(beta)/(4.0*qquad(gama)));
  term2 = fac2 * sum2;
  
  fac3 = (-4*pow(delta,-p/2.0));
  sum3 = 1 + 3*p*qquad(beta)/(2*delta);
  sum3 += p*(p+2)*qqquad(beta)/(2*quad(delta));
  term3 = fac3 * sum3;
  
  result = term1 + term2 + term3;
  
  return result;
  
}

/********************************************************************/ 










