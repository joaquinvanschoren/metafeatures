/*
 * $Source /user1/grimmer/clem/src/achim/src/RCS/entropia.c,v $
 * $Author: johann $
 * $Date: 2002/06/30 18:02:16 $
 * $Revision: 1.1.1.1 $
 */
/*
 *  $Log: entropia.c,v $
 *  Revision 1.1.1.1  2002/06/30 18:02:16  johann
 *  Initial CVS version
 *
 *  Revision 4.2  1999/06/23 18:23:31  lindner
 *  change MYLOG
 *
 * Revision 4.1  1998/07/17  11:53:10  kleiner
 * switching to revision 4.1
 *
 * Revision 3.4  1998/07/07  16:44:18  spuhl
 * changed sort-functions to fix qsort warning
 *
 * Revision 3.3  1998/07/02  08:24:21  spuhl
 * *** empty log message ***
 *
 * Revision 3.2  1998/06/29  06:30:55  spuhl
 * added results.h
 *
 * Revision 3.1  1998/06/23  12:41:29  kleiner
 * *** empty log message ***
 *
 * Revision 2.6  1998/06/22  15:59:57  kleiner
 * adjusted output
 *
 * Revision 2.5  1998/06/04  16:15:31  kleiner
 * 'isint'
 *
 * Revision 2.4  1998/05/18  07:03:57  kleiner
 * *** empty log message ***
 *
 * Revision 2.3  1998/05/17  17:39:07  Administrator
 * *** empty log message ***
 *
 *  Revision 2.2  1998/05/15 14:23:20  kleiner
 *  *** empty log message ***
 *
 * Revision 2.1  1998/05/15  07:13:34  kleiner
 * *** empty log message ***
 *
 * Revision 1.2  1998/05/07  16:18:59  kleiner
 * *** empty log message ***
 *
 */

/*********************************************************************/
/*        "Analist"  FOR PROPOSITIONAL LEARNING                      */
/*               Versao 2.0 (05/02/1997)      FILE:entropia.c        */
/*               Developed by: Joao Gama                             */
/*                LIACC - Uni.do Porto                               */
/*                jgama@ncc.up.pt                                    */
/*********************************************************************/
/***************************************/
/*  Entropy Methods                    */
/***************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "framework.h"
#include "set_var.h"
#include "results.h"
#include "measures.h" /* US added */

/*#define MYLOG(x) ( log(x) / log(2.0)) replace by function GL*/

extern short VERBOSITY;
#define VERBOSE(v) if(VERBOSITY >= v)

static double entropia_classe(DomainInfo *names, int **dist, long nr_exs);
static double entropia_atributo_discreto(DomainInfo *names, int **dist, int att, 
long nr_exs, double *H);
static double entropia_atributo_continuo(Dataset *ds, int **, int, int, int *, 
double *);

extern void Show_Instances(Dataset *ds, int low, int high);

static int NR_CLASSES;

/* new MYLOG includeed by GL */
double
MYLOG (double x) 
{   
  if (x == 0.0) 
    return 0.0;
  else 
    return (log(x) / log(2.0));
}


     

/* US moved to helpfunc.c
int
compare_attrval (AttrVal *i, AttrVal *j) {
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
*/

/***************************************/
/*  Entropy Public Methods             */
/***************************************/
void 
entropia (Dataset *ds, int ***dist, int goal) {
  register unsigned long i, j;
  int *distrib;
  double Hc, Ha = 0.0, Mca = 0.0, Hca = 0.0;
  double H = 0.0, Hy = 0.0;

  /* change koepf beginning */

  if (!REGRESSION){

    NR_CLASSES = NValsAttr(ds->domain, goal);
    distrib = ivector(1, NR_CLASSES);
    Hc = entropia_classe(ds->domain, dist[goal], ds->nr_exs);
    fprintf (stdout, "\nClassEntropy: %7.4f\n", Hc);

    results[CLASS_ENTR] = Hc;
  }

  else{
  ;
  }

  /* change koepf end */

  for(i = 1; i <= NrAttrs(ds->domain); i++) {
    if (i != goal) {
      switch(TypeAttr(ds->domain, i)) {
      case continuous: 
	fprintf (stderr, "*** entropia: Using qsort instead of QuickSort\n");
	qsort (&ds->instances[i][1], ds->nr_exs, sizeof (AttrVal), compare_attrval); 
	fprintf (stderr, "*** entropia: Used qsort instead of QuickSort (successfully\n");
/*	QuickSort(ds, i, 1, ds->nr_exs); */
	Show_Instances(ds, 1, ds->nr_exs); 
	Hy = entropia_atributo_continuo(ds, dist[goal], i, goal, distrib, &H);
	VERBOSE(2) {
	  fprintf (stdout, "Entropy Attribute (%s) = %7.4f  I_gain_ratio = %7.4f\n", 
		   ds->domain->attrs[i].name, Hy, (Hc - Hy)/H); 
	  fflush (stdout);
	}
	for(j = 1; j <= NR_CLASSES; j++) distrib[j] = 0;
	break;
      case ordered:
      case nominal:
      case integer:
	Hy = entropia_atributo_discreto(ds->domain, dist[i], i, ds->nr_exs, &H);
	VERBOSE(2) {
	  fprintf (stdout, "Entropy Attribute (%s) = %7.4f  I_gain_ratio = %7.4f\n", 
		   ds->domain->attrs[i].name, Hy, (Hc - Hy)/H); 
	  fflush (stdout);
	}
	break;
      case structured:  /* (TK): added to avoid warning */
	break;
      }
      Ha += Hy;
      Mca += Hc + Hy - H;
      Hca += H;
    }
  }

  /* change koepf beginning */

  if (((REGRESSION) && (ONLY_NUMERIC)) || ((!REGRESSION) && (ONLY_NUMERIC))){
    ;
  }

  else if ((REGRESSION) && (!ONLY_NUMERIC)){

    Ha /= (double) NrAttrs(ds->domain) - 1.0;
    fprintf (stdout, "\nEntropyAttributes:   %8.4f\n", Ha);
    results[ENTR_OF_ATTR] = Ha;

    fflush (stdout);
  }

  else {

  /* change koepf end */

  Ha /= (double) NrAttrs(ds->domain) - 1.0;
  Mca /= (double) NrAttrs(ds->domain) - 1.0;
  Hca /= (double) NrAttrs(ds->domain) - 1.0;
  fprintf (stdout, "\nEntropyAttributes:   %8.4f\n", Ha);
  fprintf (stdout, "MutualInformation:      %8.4f\n", Mca);
  fprintf (stdout, "JointEntropy:           %8.4f\n", Hca);
  fprintf (stdout, "Equivalent_nr_of_attrs: %8.4f\n", Hc / Mca);
  fprintf (stdout, "NoiseSignalRatio:      %8.4f\n", (Ha - Mca) / Mca);

  results[ENTR_OF_ATTR] = Ha;
  results[MUT_INFO] = Mca;
  results[JOINT_ENTR] = Hca;
  results[EQUI_NR_ATTR] = Hc / Mca;
  results[NOISE_SIG_RAT] = (Ha - Mca) / Mca;

  fflush (stdout);
  }

}
/***************************************/
/*  Entropy Private Methods            */
/***************************************/
static double entropia_classe(DomainInfo *names, int **dist, long nr_exs)
{
  register int i;
  double p, ent_cl = 0.0;

  for(i = 1; i <= NR_CLASSES; i++) {
    p = (double) dist[i][i] / (double) nr_exs;
    if (p == 0) { /* test auf 0 for log GL */
      ent_cl-= 0;
               }
    else 
      {
	ent_cl -= p * MYLOG(p);
      }

  }
  return ent_cl;
}

static double entropia_atributo_discreto\
(DomainInfo *names, int **dist, int att, long nr_exs, double *H)
{
  register int i, j;
  double p, ent_cl = 0.0;

  for(i = 1; i <= NValsAttr(names,att); i++) 
    if ( dist[0][i]) {
      p = (double) dist[0][i] / (double) nr_exs;
      ent_cl -= p * MYLOG(p);
    }
  *H = 0.0;
  for(i = 1; i <= NValsAttr(names,att); i++) 
    for (j = 1; j <= NR_CLASSES; j++) 
      if (dist[j][i]) {
	p = (double) dist[j][i] / (double) nr_exs;
	*H -= p * MYLOG(p);
      }
  return ent_cl;  
}

static double entropia_atributo_continuo\
(Dataset *ds, int **dist, int att,  int goal, int *distrib, double *H)
{
  register int i = 1, j, classe; /* (TK): unused: sum = 0 */
  char dummy[128];
  double p, split_value, ent_att = 0.0, best_ent = 0.0; /* (TK): unused: best_H */

  while(i < ds->nr_exs) {
    split_value = CValAttEx(ds->instances[i], att);

    while(i < ds->nr_exs && split_value == CValAttEx(ds->instances[i], att)) {
      classe = DValAttEx(ds->instances[i++], goal);
      if (TypeAttr(ds->domain, goal) == integer) {
        sprintf(dummy, "%d", classe);
        classe = IdValLbl(ds->domain, goal, dummy);
      }
      ++distrib[classe];
    }

    if (i < ds->nr_exs) {
      p = (double) i / (double) ds->nr_exs;
      ent_att = -1.0 * p * MYLOG(p);
      p = (double) (ds->nr_exs - i) / (double) ds->nr_exs;
      ent_att -=  p * MYLOG(p);
    }
/*
printf("n %d Inf=%.3f Spli:%.3f\t", i, ent_att, split_value  );
for(j = 1; j <= NR_CLASSES; j++) printf("%d ", distrib[j]);
printf("\n");
*/
    if (p && ent_att > best_ent) {
      best_ent = ent_att;
      *H = 0.0;
      for(j = 1; j <= NR_CLASSES; j++) {
	if (distrib[j]) {
	  p = (double) distrib[j] / (double) ds->nr_exs;
	  *H -= p * MYLOG(p);
	}
	if (dist[j][j] - distrib[j]) {
	  p = (double) (dist[j][j] -distrib[j]) / (double) ds->nr_exs;
	  *H -= p * MYLOG(p);
	}
      }
/*      printf("Best  Inf=%.3f Spli:%.3f Gain %.3f\n", best_ent, split_value,*H  ); */
    }
  }
/*  printf("Via sair  Inf=%.3f Gain %.3f\n", best_ent, *H  ); */
  return best_ent;
}
