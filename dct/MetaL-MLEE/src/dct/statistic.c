/*
 * $Source /user1/grimmer/clem/src/achim/src/RCS/statistic.c,v $
 * $Author: johann $
 * $Date: 2002/06/30 18:02:16 $
 * $Revision: 1.1.1.1 $
 */
/*
 *  $Log: statistic.c,v $
 *  Revision 1.1.1.1  2002/06/30 18:02:16  johann
 *  Initial CVS version
 *
 *  Revision 4.1  1998/07/17 11:53:46  kleiner
 *  switching to revision 4.1
 *
 * Revision 3.4  1998/07/15  15:10:58  spuhl
 * *** empty log message ***
 *
 * Revision 3.3  1998/07/02  08:24:43  spuhl
 * *** empty log message ***
 *
 * Revision 3.2  1998/06/29  06:39:01  spuhl
 * added results.h
 * all code for saving Measures.data removed
 *
 * Revision 3.1  1998/06/23  12:41:42  kleiner
 * *** empty log message ***
 *
 * Revision 2.7  1998/06/06  17:07:30  thor
 * removed counting of missing values
 * is now done by instances.c
 *
 *  Revision 2.6  1998/05/18 07:04:12  kleiner
 *  *** empty log message ***
 *
 * Revision 2.5  1998/05/17  18:22:27  Administrator
 * *** empty log message ***
 *
 *  Revision 2.4  1998/05/17 17:46:56  Administrator
 *  *** empty log message ***
 *
 *  Revision 2.3  1998/05/17 17:39:30  Administrator
 *  *** empty log message ***
 *
 *  Revision 2.2  1998/05/15 14:23:32  kleiner
 *  *** empty log message ***
 *
 * Revision 2.1  1998/05/15  07:13:48  kleiner
 * *** empty log message ***
 *
 * Revision 1.3  1998/05/13  13:45:53  kleiner
 * version 2.2
 *
 * Revision 1.2  1998/05/07  16:19:12  kleiner
 * *** empty log message ***
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "framework.h"
#include "measures.h"
#include "set_var.h"
#include "results.h"  /* US */


/****************************************************
   ShowStat shows the following statistical information:
   nr  attributes, nr numeric attributes, nr symbolic attributes,
   nr examples, nr classes, prior probabilities for all classes, 
   name and accurancy of default class 

Input:
  sym: Dataset of symbolic attributes
  num: Dataset of numeric attributes
  dist: Distribution of symblic attributes
Output:
  void, shows statistical information 
*****************************************************/

void 
ShowStat(Dataset *sym, Dataset *num, int ***dist) {

  int nr, nr_cl,i,ind, nr_s, nr_n, nr_a, j;
  int sum, tmp_cl, max_cl, min_cl;
  char *name;
  double *vec;
  double max =  0;
  double stddevcl;
 
  /* (TK): unused: int file = 0; */
  /* (TK): unused: extern int UNKNOWN; */
  /* (TK): extern int NR_OUTLIERS; */

  /* change koepf beginning */

  if (REGRESSION){

    fprintf (stdout,"\n#*** Statistic ***\n");
    nr_a =  (num -> domain -> nr_attrs)+(sym -> domain -> nr_attrs) - 1;
    fprintf (stdout,"\nNr_attributes: %d\n#(including target attribute)\n", nr_a);
 
    nr_s = sym -> domain -> nr_attrs;
    fprintf (stdout,"\nNr_sym_attributes: %d", nr_s);
  
    nr_n = num -> domain -> nr_attrs - 2;
    fprintf (stdout,"\nNr_num_attributes: %d\n", nr_n);
  
    fprintf (stdout,"\nNr_examples: %ld\n", num -> nr_exs);
  }
  else{

  /* change koepf end */

  fprintf (stdout,"\n#*** Statistic ***\n");
  nr_a =  (num -> domain -> nr_attrs)+(sym -> domain -> nr_attrs) - 1;
  fprintf (stdout,"\nNr_attributes: %d\n#(including target attribute)\n", nr_a);
 
  nr_s = sym -> domain -> nr_attrs - 1;
  fprintf (stdout,"\nNr_sym_attributes: %d", nr_s);
  
  nr_n = num -> domain -> nr_attrs - 1;
  fprintf (stdout,"\nNr_num_attributes: %d\n", nr_n);
  
  fprintf (stdout,"\nNr_examples: %ld", num -> nr_exs);

  nr_cl = NValsAttr(sym -> domain, sym -> domain -> nr_attrs);
  fprintf (stdout,"\nNr_classes:  %d\n", nr_cl);

  name = sym -> domain -> attrs[sym->domain->nr_attrs].name;
  nr = sym -> domain -> nr_attrs;

  vec = vector_mem(nr_cl);
  for(i = 1; i <= nr_cl; i++) {
    vec[i-1] = dist[nr][0][i]/(double)(sym->nr_exs);
    if (vec[i-1] > max) {
      max = vec[i-1];
      ind = i;
    } 
    fprintf (stdout,"ClassValue_%d_Name: %s\n",i,LblValId(sym -> domain,nr,i));
    fprintf (stdout,"ClassValue_%d_Count: %d\n",i,dist[nr][0][i]);
    fprintf (stdout,"ClassValue_%d_Freq: %1.3f\n",i,vec[i-1]);
  }

/*
 *  for(i = 1; i <= nr_cl; i++){
 *  for(j = 0; j <= 1; j++) {
 *    for(nr = 1; nr <= 2; nr++) {
 *	printf("\n\n Werte von dist[%d][%d][%d] sind %d\n\n", nr, j, i, dist[nr][j][i]);
 *    }
 *  }
 *}
 */

  fprintf (stdout,"\n");
  fprintf (stdout,"\nDefaultClass: %d\n",ind);
  fprintf (stdout,"\nDefaultAccuracy: %1.5f\n",vec[ind-1]);
  

  results[NR_SYM_ATTR] = (double) nr_s;
  results[NR_NUM_ATTR] = (double) nr_n;
  results[NR_ATTR] = (double) (nr_s + nr_n);
  results[NR_TARGET_CL] = (double) nr_cl;
  results[DEF_ACCURACY] = vec[ind-1];

  stddevcl = standarddev(vec,nr_cl);           /* standard deviation of prior prob. */
  results[STD_DEV_CL] =  stddevcl;              /* of classes                        */  

  fprintf(stdout,"Class_Prob_StdDev: %1.5f\n");

 /* change koepf beginning */

  }

  if (nr_n == (nr_a - 1)){
    ONLY_NUMERIC = TRUE;
  }

 /* end change koepf */

  if (nr_s) {
    min_cl = max_cl = NValsAttr(sym->domain, 1);
    sum = min_cl;
    for(i = 2; i <= nr_s; i++) {     
      tmp_cl = NValsAttr(sym -> domain,i);
      if (tmp_cl < min_cl) {
	min_cl = tmp_cl;
      }
      else if (tmp_cl > max_cl) {
	max_cl = tmp_cl;
      }
      sum += tmp_cl;
    } 
    results[SYM_CL_AVG] = (double) sum / (double) nr_s;	/* avg. of diff. values     */
    results[SYM_CL_MIN] = (double) min_cl;
    results[SYM_CL_MAX] = (double) max_cl;
	fprintf(stdout,"NSym_Attr_Avg: %5.3f\n",results[SYM_CL_AVG]);
	fprintf(stdout,"NSym_Attr_Min: %5.3f\n",results[SYM_CL_MIN]);
	fprintf(stdout,"NSym_Attr_Max: %5.3f\n",results[SYM_CL_MAX]);

  }
  fflush (stdout);
  
}                             
 
void 
ShowStat_old(Dataset *sym, Dataset *num, int ***dist) {

  int nr, nr_cl,i,ind, nr_s, nr_n, nr_a;
  int sum, tmp_cl, max_cl, min_cl;
  char *name;
  double *vec;
  double max =  0;
  double stddevcl;
  /* (TK): unused: int file = 0; */
  /* (TK): unused: extern int UNKNOWN; */
  /* (TK): extern int NR_OUTLIERS; */

  fprintf (stdout,"\n#*** Statistic ***\n");
  nr_a=  (num -> domain -> nr_attrs)+(sym -> domain -> nr_attrs)-1;
  fprintf (stdout,"\nNr_attributes: %d\n#(including target attribute)\n", nr_a);
 
  nr_s = sym -> domain -> nr_attrs - 1;
  fprintf (stdout,"\nNr_sym_attributes: %d", nr_s);
  
  nr_n = num -> domain -> nr_attrs -1;
  fprintf (stdout,"\nNr_num_attributes: %d\n", nr_n);
  
  fprintf (stdout,"\nNr_examples: %ld", num -> nr_exs);
  
  nr_cl = NValsAttr(sym -> domain, sym -> domain -> nr_attrs);
  fprintf (stdout,"\nNr_classes:  %d\n", nr_cl);
  
  
  name = sym -> domain -> attrs[sym->domain->nr_attrs].name;
  nr = sym -> domain -> nr_attrs;
  fprintf (stdout,"\nClass attribute = %s\n", name);  
  fprintf (stdout,"\n%20s %8s %8s", "Frequency:", "absolute", "relative");
  
  vec = vector_mem(nr_cl);
  for(i = 1; i <= nr_cl; i++) {
    vec[i-1] = dist[nr][0][i]/(double)(sym->nr_exs);
    if (vec[i-1] > max) {
      max = vec[i-1];
      ind = i;
    } 
    fprintf (stdout,"\n%20s: %8d %8.3f",LblValId(sym -> domain,nr,i),dist[nr][0][i], vec[i-1]);
  }
  fprintf (stdout,"\n");
  fprintf (stdout,"\nDefault class is %s (%.1f%%)\n", LblValId(sym -> domain,nr,ind), vec[ind-1]*100); 

  results[NR_SYM_ATTR] = (double) nr_s;
  results[NR_NUM_ATTR] = (double) nr_n;
  results[NR_ATTR] = (double) (nr_s + nr_n);
  results[NR_TARGET_CL] = (double) nr_cl;
  results[DEF_ACCURACY] = vec[ind-1];

  stddevcl = standarddev(vec,nr_cl);           /* standard deviation of prior prob. */
  results[STD_DEV_CL] =  stddevcl;              /* of classes                        */  

  if (nr_s) {
    min_cl = max_cl = NValsAttr(sym->domain, 1);
    sum = min_cl;
    for(i = 2; i <= nr_s; i++) {     
      tmp_cl = NValsAttr(sym -> domain,i);
      if (tmp_cl < min_cl) {
	min_cl = tmp_cl;
      }
      else if (tmp_cl > max_cl) {
	max_cl = tmp_cl;
      }
      sum += tmp_cl;
    } 
    results[SYM_CL_AVG] = (double) sum / (double) nr_s;	/* avg. of diff. values     */
    results[SYM_CL_MIN] = (double) min_cl;
    results[SYM_CL_MAX] = (double) max_cl;     	
  }
  fflush (stdout);
  
}                             
 




























