/*
 * $Source /user1/grimmer/clem/src/achim/src/RCS/utils.c,v $
 * $Author: johann $
 * $Date: 2002/06/30 18:02:16 $
 * $Revision: 1.1.1.1 $
 */
/*
 *  $Log: utils.c,v $
 *  Revision 1.1.1.1  2002/06/30 18:02:16  johann
 *  Initial CVS version
 *
 *  Revision 4.1  1998/07/17 11:53:51  kleiner
 *  switching to revision 4.1
 *
 * Revision 3.4  1998/07/15  15:11:10  spuhl
 * unused functions deleted
 *
 * Revision 3.3  1998/07/02  17:09:56  spuhl
 * fixed german output
 * fixed output of target field
 *
 * Revision 3.2  1998/07/02  08:24:46  spuhl
 * *** empty log message ***
 *
 * Revision 3.1  1998/06/23  12:41:45  kleiner
 * *** empty log message ***
 *
 * Revision 2.7  1998/06/12  12:26:03  thor
 * changed ShowDistributions
 *
 *  Revision 2.6  1998/05/27 14:07:47  kleiner
 *  *** empty log message ***
 *
 * Revision 2.5  1998/05/18  07:04:15  kleiner
 * *** empty log message ***
 *
 * Revision 2.4  1998/05/17  17:47:02  Administrator
 * *** empty log message ***
 *
 *  Revision 2.3  1998/05/17 17:39:35  Administrator
 *  *** empty log message ***
 *
 *  Revision 2.2  1998/05/15 14:23:33  kleiner
 *  *** empty log message ***
 *
 * Revision 2.1  1998/05/15  07:13:51  kleiner
 * *** empty log message ***
 *
 * Revision 1.2  1998/05/07  16:19:15  kleiner
 * *** empty log message ***
 *
 */

/*********************************************************************/
/*        FRAME WORK FOR PROPOSITIONAL LEARNING                      */
/*        Versao 1.0 (10/12/1996)      FILE:utils.c                  */
/*  Developed by: Joao Gama, Luis Torgo                              */
/*                LIACC - Uni.do Porto                               */
/*                {jgama, ltorgo}@ncc.up.pt                          */
/*********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <float.h>
#include "framework.h"

#define SQR(x) ((x) * (x))
#define STD(x, x2,n) sqrt((x2 - 2 * x * (x / n) + n * (x / n) * (x/ n)) / n)

static int ***init_distributions(DomainInfo *domain, int nr);
/*static double ***init_moments(DomainInfo *domain, int nr_cl, int nr_at);*/
/**************************************/
/*   Public Methods  Distributions    */
/**************************************/
int ***Distribuitions(Dataset *ds, int goal, unsigned long Low, \
		      unsigned long High)
{
  register int classe, valor, j, nr_classes, pos; /* TK added int */
  char dummy[128];
  long i;
  int ***stat;
  
  nr_classes = NValsAttr(ds->domain, goal);
  stat = init_distributions(ds->domain, nr_classes);
  for(i = Low; i <= High; i++) {
    if(nr_classes) {
      classe = DValAttEx(ds->instances[i], goal);
      if (TypeAttr(ds->domain, goal) == integer) {
	sprintf(dummy, "%d", classe);
	classe = IdValLbl(ds->domain, goal, dummy);
      }
    }
    for(j = 1; j <= NrAttrs(ds->domain); j++) {
      if(NormalVal(ds->instances[i][j])) {
	switch(TypeAttr(ds->domain, j)) {
	case nominal:
	case ordered:
	  valor = DValAttEx(ds->instances[i], j);
	  ++stat[j][0][valor];
	  if(nr_classes) ++stat[j][classe][valor];
	  break;
	case integer:
	  valor = DValAttEx(ds->instances[i], j);
	  sprintf(dummy, "%d", valor);
	  pos = IdValLbl(ds->domain, j, dummy);
	  ++stat[j][0][pos];
	  if(nr_classes) ++stat[j][classe][pos];
	  break;
	case continuous:
	  break;
	default:  /* (TK): added to avoid warnings */
	  break;
	}
      }
      else if (UnknownVal(ds->instances[i][j])) {
	stat[j][classe][NValsAttr(ds->domain, j) +1 ]++;
      }
    }
  }
  return stat;
} 


void 
ShowDistributions_old(DomainInfo *domain, int ***dist, int goal, 
		  unsigned long nr_exs) {
  register int j, i, k, nr_classes; /* (TK): unused: n */

  int l; /* TK */

  nr_classes = NValsAttr(domain, goal);
  fprintf(stdout, "\n#*** Attribute Distribution ***\n");		  /*          NEW             */            
   for(j = 1; j < NrAttrs(domain); j++) {                /*  don't show distribution */
     switch(TypeAttr(domain, j)) {                        /*  of target attribute     */
     case nominal:
     case ordered:
     case integer:
       fprintf (stdout, "\nAttribute: %s %s\n", NameAttr(domain, j), j == goal ? "[TARGET]" : " ");
       fprintf (stdout, "         ");
       for(i = 1; i <= NValsAttr(domain, j); i++)
	 fprintf (stdout, "%16s  ", LblValId(domain, j,i));
       fprintf (stdout, "\n");
       for(i = 0; i <= nr_classes; i++) {
	 if (i) {
	   fprintf (stdout, "%.8s ", LblValId(domain,goal, i));
	   if (strlen(LblValId(domain,goal,i)) < 8) {
	     for (l=0; l<8-strlen(LblValId(domain,goal,i)); l++) {
	       fprintf (stdout, " ");
	     }
	   }
	 }
	 else   fprintf (stdout, "DS:      ");
	 for(k = 1; k <= NValsAttr(domain, j); k++) {
	   fprintf (stdout, "%8d ", dist[j][i][k]);
	   if (i == 0) {
	     fprintf (stdout, "(%4.1f%%)  ", 
		      (double) dist[j][i][k]*100/nr_exs);
	   }
	   else {
	     fprintf (stdout, "(%4.1f%%)  ", 
 		      (double) dist[j][i][k]*100/dist[goal][0][i]); 
	   }
	 }
	 fprintf (stdout, "\n");
       }
       break;
     default:  /* (TK): added to avoid warnings */
       break;
     }
   }
  fflush (stdout);

}


void 
ShowDistributions(DomainInfo *domain, int ***dist, int goal, 
		  unsigned long nr_exs) {
  register int j, i, k, nr_classes,AttrNum; /* (TK): unused: n */

  int l; /* TK */

  /* change koepf */

  if (REGRESSION == TRUE){
      fprintf(stdout, "\n#*** Attribute Distribution ***\n");
      for(j = 1; j < (NrAttrs(domain) + 1); j++) {             
	switch(TypeAttr(domain, j)) {                        
	case nominal:
	case ordered:
	case integer:
	  AttrNum=GlobalNum(domain,j);	
	  fprintf (stdout, "Attr_Name_%d: %s\n",AttrNum, NameAttr(domain,j));
	  fprintf (stdout, "Attr_Type_%d: discrete\n",AttrNum);
	  i = 0;

	    for(k = 1; k <= NValsAttr(domain, j); k++) {
	        fprintf (stdout, "Attr_Count_All_Value_%d_%d: %d\n",k,AttrNum,dist[j][i][k]);
		fprintf (stdout, "Attr_Freq_All_Value_%d_%d: %4.1f\n",k,AttrNum,(double) dist[j][i][k]*100/nr_exs);
	    }	  
	  break;
	default:  /* (TK): added to avoid warnings */
	  break;
	}
      }
      fflush (stdout);
  }
  else {

  /* end change koepf */

  nr_classes = NValsAttr(domain, goal);
  fprintf(stdout, "\n#*** Attribute Distribution ***\n");		  /*          NEW             */     
   for(j = 1; j < NrAttrs(domain); j++) {                /*  don't show distribution */
     switch(TypeAttr(domain, j)) {                        /*  of target attribute     */
     case nominal:
     case ordered:
     case integer:
       AttrNum=GlobalNum(domain,j);	
       fprintf (stdout, "Attr_Name_%d: %s\n",AttrNum, NameAttr(domain,j));
       fprintf (stdout, "Attr_Type_%d: discrete\n",AttrNum);

       for(i = 0; i <= nr_classes; i++) {
	 for(k = 1; k <= NValsAttr(domain, j); k++) {
	   if (i == 0) {
	        fprintf (stdout, "Attr_Count_All_Value_%d_%d: %d\n",k,AttrNum,dist[j][i][k]);
		fprintf (stdout, "Attr_Freq_All_Value_%d_%d: %4.1f\n",k,AttrNum,(double) dist[j][i][k]*100/nr_exs);
	   }
	   else {
	    	fprintf (stdout, "Attr_Count_Cl%d_Value_%d_%d: %d\n",i,k,AttrNum,dist[j][i][k]);
		fprintf (stdout, "Attr_Freq_Cl%d__Value_%d_%d: %4.1f\n",i,k,AttrNum,(double) dist[j][i][k]*100/dist[goal][0][i]);
	   }
	 }
	 //fprintf (stdout, "\n");
       }
       break;
     default:  /* (TK): added to avoid warnings */
       break;
     }
   }
  fflush (stdout);

  /* change koepf */

  }

  /* end change koepf */
}


/**************************************/
/*   Private Methods  Distributions   */
/**************************************/
static int ***init_distributions(DomainInfo *domain, int nr)
{
  register int i, c = 0;
  int ***stat, **att;  /* (TK): unused: , *means; */
  int j, k;
  
  if (nr > 1)  c = nr + 1;
  stat = (int ***) malloc((int) NrAttrs(domain) * sizeof(int **));  
  --stat;
  for(i = 1; i <= NrAttrs(domain); i++) {
    switch(TypeAttr(domain, i)) {
    case integer:
    case nominal:
    case ordered:
    case structured:
      att = imatrix(0, c, 0, 2 + NValsAttr(domain, i)); /* US one more for missing values */
      for (j = 0; j < c; j++)
	for (k = 0; k < 2 + NValsAttr(domain, i); att[j][k++] = 0);
      break;
    case continuous:
      att = NULL;
      break;
    }
    stat[i] = att;
  }
  return stat;
}
