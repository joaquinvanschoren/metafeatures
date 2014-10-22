/*
 * $Source /user1/grimmer/clem/src/achim/src/RCS/estatistica.c,v $
 * $Author: johann $
 * $Date: 2002/06/30 18:02:16 $
 * $Revision: 1.1.1.1 $
 */
/*
 * Revision 5.2 2001/03/09 15:21:10 koepf
 * changed name and calculation of 
 * Mean_Skew to Mean_Absolute_skew
 *
 * Revision 5.1 2001/02/23 17:22:15 koepf
 * added ShowCovMatrix
 *
 *  $Log: estatistica.c,v $
 *  Revision 1.1.1.1  2002/06/30 18:02:16  johann
 *  Initial CVS version
 *
 *  Revision 4.1  1998/07/17 11:53:13  kleiner
 *  switching to revision 4.1
 *
 * Revision 3.5  1998/07/15  15:10:39  spuhl
 * *** empty log message ***
 *
 * Revision 3.4  1998/07/02  08:24:22  spuhl
 * *** empty log message ***
 *
 * Revision 3.3  1998/06/29  06:31:57  spuhl
 * added results.h
 *
 * Revision 3.2  1998/06/24  08:44:27  spuhl
 * adjusted error in calculating confidence matrix
 *
 * Revision 3.1  1998/06/23  12:41:30  kleiner
 * *** empty log message ***
 *
 * Revision 2.8  1998/06/04  16:15:36  kleiner
 * 'isint'
 *
 * Revision 2.7  1998/06/04  10:49:24  kleiner
 * changed MyShowMoments  (confidence intervals for mean)
 *
 * Revision 2.6  1998/05/28  13:43:50  kleiner
 * now checking if jacobi was succesful
 *
 * Revision 2.5  1998/05/27  14:06:23  kleiner
 * aligned some output
 * NAttVals ...
 *
 * Revision 2.4  1998/05/18  07:03:58  kleiner
 * *** empty log message ***
 *
 * Revision 2.3  1998/05/17  17:39:10  Administrator
 * *** empty log message ***
 *
 *  Revision 2.2  1998/05/15 14:23:23  kleiner
 *  *** empty log message ***
 *
 * Revision 2.1  1998/05/15  07:13:35  kleiner
 * *** empty log message ***
 *
 * Revision 1.5  1998/05/14  16:40:15  kleiner
 * *** empty log message ***
 *
 * Revision 1.4  1998/05/13  13:45:32  kleiner
 * version 2.2
 *
 * Revision 1.3  1998/05/12  14:57:41  kleiner
 * added output
 *
 * Revision 1.2  1998/05/07  16:19:00  kleiner
 * *** empty log message ***
 *
 */

/*********************************************************************/
/*        "Analist"  FOR PROPOSITIONAL LEARNING                      */
/*               Versao 2.0 (05/02/1997)     FILE:estatistica.c      */
/*               Developed by: Joao Gama                             */
/*                LIACC - Uni.do Porto                               */
/*                jgama@ncc.up.pt                                    */
/*********************************************************************/
/***************************************/
/*  Statistical Methods                */
/***************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <float.h>
#include "framework.h"
#include "measures.h"
#include "estatistica.h"
#include "set_var.h"
#include "results.h"

double M_SPANNW = 0;
extern short VERBOSITY;
#define VERBOSE(v) if(VERBOSITY >= v)

/* #define ABS(x) (x > 0 ? x : -1.0 * x) */
#define SQR(x) ((x) * (x))
#define STD(x, x2,n)           sqrt((x2-2*x*(x/n)+n*(x/n)*(x/n))/n)
#define SKEW(a3,a2,a,m,s,n)    (a3+m*(-3*a2+m*(3*a-n*m)))/(n*s*s*s)
#define KURT(a4,a3,a2,a,m,s,n) (a4+m*(-4*a3+m*(6*a2+m*(-4*a+n*m))))/(n*s*s*s*s)

static int NR_CLASSES;
static double ***init_moments(DomainInfo *domain, int nr_cl, int nr_at);
static double m_statistics(double **si, double **s, int m);
/***************************************/
/*  Statistical Public Methods         */
/***************************************/
double **
correlacoes (Dataset *ds, int goal, double ***momentos, double ***covar) {
  register unsigned long i, j, k, ati, atj;
  double di, dj, factor, correl_abs = 0.0;
  double **correl = dmatrix(1, NrAttrs(ds->domain) - 1, 1, 
			    NrAttrs(ds->domain)-1);
  double **global = dmatrix(1, NrAttrs(ds->domain) - 1, 1, 
			    NrAttrs(ds->domain)-1);
  int nr;

  for(k = 1; k <= NR_CLASSES; k++) {
    for(i = 1; i < NrAttrs(ds->domain); i++) 
      for(j = 1; j <= i; j++) {
	if( i != goal && j != goal) {
	  ati = i < goal ? i : i - 1;
	  atj = j < goal ? j: j - 1;
	  correl[ati][atj] = 0.0;
	  switch(TypeAttr(ds->domain, i)) {
	  case continuous: 
	    di = momentos[i][k][4] == 0.0 ? momentos[i][0][4] : 
	    momentos[i][k][4];
	    break;
	  case ordered:
	  case nominal:
	  case integer:
	    di = momentos[i][k][2] == 0.0 ? momentos[i][0][2] : 
	    momentos[i][k][2];
	    break;
	  case structured:	/* (TK) */
	    break;
	  }
	  switch(TypeAttr(ds->domain, j)) {
	  case continuous: 
	    dj = momentos[j][k][4] == 0.0 ? momentos[j][0][4] : 
	    momentos[j][k][4];
	    break;
	  case ordered:
	  case nominal:
	  case integer:
	    dj = momentos[j][k][2] == 0.0 ? momentos[j][0][2] : 
	    momentos[j][k][2];
	    break;
	  case structured:	/* (TK) */
	    break;
	  }
	  if (di * dj) {
	    if (ati == atj)  factor = covar[k][ati][atj] / (di * dj);
	    if (factor) correl[ati][atj] += covar[k][ati][atj] / (di * dj * 
								  factor);
	    global[ati][atj] += correl[ati][atj];
	  }
	  else
	    fprintf(stderr,"Correlation Attribute %ld %ld Standard devion 0 !", j, 
		    i);
	  	 /* printf("Classe %ld Ati %ld Atj %ld Di %.5f Dj %.5f\n", k, i, j, di, dj); */
	}
      }
      
    VERBOSE(3) {
      fprintf (stdout, "\n");  
      nr = ds -> domain -> nr_attrs;
      fprintf (stdout, "Correlation Matrix Class %ld (%s)\n", k, LblValId(ds -> domain,nr,k));
      Show_Correl(correl, NrAttrs(ds->domain)-1);
      fprintf (stdout, "\n");
    }
  }
  for(i = 1; i < NrAttrs(ds->domain); i++) 
    for(j = 1; j <= i; j++) {
      global[i][j] /= NR_CLASSES;
      if (j < i) correl_abs += fabs(global[i][j]);
    }
  fprintf (stdout, "\nGlobal Correlation Matrix (Average: %.5f)\n", correl_abs / 
	 ((NrAttrs(ds->domain)-1.0) * NrAttrs(ds->domain) / 2.0));
  VERBOSE(2) Show_Correl(global, NrAttrs(ds->domain)-1);

  fflush (stdout);

  return correl;
}

double **
correlacao(Dataset *ds, double ***momentos, double **covar)
{
  register int i, j;		/* (TK): unused: , k; */
  double di = 0, dj = 0, factor = 1;	/* (TK): unused: , correl_abs = 0.0; */
  double **correl = dmatrix(1, NrAttrs(ds->domain)-1, 1, NrAttrs(ds->domain)-1);

  for(i = 1; i < NrAttrs(ds->domain); i++) 
    for(j = 1; j <= i; j++) {
      switch(TypeAttr(ds->domain, i)) {
      case continuous: 
	di = momentos[i][0][4];
	break;
      case ordered:
      case nominal:
      case integer:
	di = momentos[i][0][2];
	break;
      case structured:		/* (TK) */
	break;
      }
      switch(TypeAttr(ds->domain, j)) {
      case continuous: 
	dj = momentos[j][0][4];
	break;
      case ordered:
      case nominal:
      case integer:
	dj = momentos[j][0][2];
	break;
      case structured:		/* (TK) */
	break;
      }

      if (di * dj) {
	if (i == j)  factor = covar[i][j] / (di * dj);
	else factor = 1;
	correl[i][j] += covar[i][j] / (di * dj * factor);
      }
      else
	fprintf(stderr, "Correlation Attribute %d Standard devion 0 !", di ? j : 
		i);
    }
  VERBOSE(2) {
    fprintf (stdout, "Correlation Matrix\n");
    Show_Correl(correl, NrAttrs(ds->domain)-1);
  }
  
  fflush (stdout);
  
  return correl;
}

void 
Show_Correl(double **m, int n) {
  register int i, j;

  for(i = 1; i <= n; i++) fprintf (stdout, "\t   [%d]\t", i);
  fprintf (stdout, "\n");
  for(i = 1; i <= n; i++) {
    fprintf (stdout, "[%d]\t", i);
    for(j = 1; j <= i; j++) 
      fprintf (stdout, "%9.5f\t", m[i][j]);
    fprintf (stdout, "\n");
  }
  fflush (stdout);
}

double ***
covariance (Dataset *ds, int goal, double ***momentos, int **classes) {
  register int i, j, k, atj, atk, classe;
  char dummy[128];
  double x, y, mx, my;
  double ***cov;

  NR_CLASSES = NValsAttr(ds->domain, goal);
  cov = dtensor(2+NR_CLASSES, NrAttrs(ds->domain)-1, NrAttrs(ds->domain)-1);
  /* ************************************************
     Compute Class covariance matrices from Data
     *************************************************** */
  for (i = 1; i <= ds->nr_exs; i++) {
    classe = DValAttEx(ds->instances[i], goal);
    if (TypeAttr(ds->domain, goal) == integer) {
      sprintf(dummy, "%d", classe);
      classe = IdValLbl(ds->domain, goal, dummy);
    }
    for(j = 1; j < NrAttrs(ds->domain); j++) {
      if (goal != j) {
	switch(TypeAttr(ds->domain, j)) {
	case continuous: 
	  x = CValAttEx(ds->instances[i], j); 
	  mx = momentos[j][classe][3];
	  break;
	case integer:
	case ordered:
	case nominal:
	  x = (double) DValAttEx(ds->instances[i], j); 
	  mx = momentos[j][classe][1];
	  break;
	case structured:	/* (TK) */
	  break;
	}
	for(k = j; k < NrAttrs(ds->domain); k++) {
	  if (goal != k) {
	    switch(TypeAttr(ds->domain, k)) {
	    case continuous: 
	      y = CValAttEx(ds->instances[i], k); 
	      my = momentos[k][classe][3];
	      break;
	    case ordered:
	    case integer:
	    case nominal:
	      y = (double) DValAttEx(ds->instances[i], k); 
	      my = momentos[k][classe][1];
	      break;
	    case structured:	/* (TK) */
	      break;
	    }
	    if (j > goal) atj = j - 1;     else          atj = j;
	    if (k > goal) atk = k - 1;     else          atk = k;
	    cov[classe][atj][atk] += (x - mx) * (y - my);
	    cov[NR_CLASSES+1][atj][atk] += (x - momentos[k][0][3]) * (y - 
								      momentos[k][0][3]);
	  }
	}
      }
    }
  }
  /* ************************************************
     Normalize matrices and 
     compute Pooled covariance Matrix
     *************************************************** */
  for(k = 1; k <= NR_CLASSES; k++)
    for(i = 1; i < NrAttrs(ds->domain); i++)
      for(j = i; j < NrAttrs(ds->domain); j++) {
	if ((classes[k][k] - 1.0) == 0) {
	  fprintf (stdout, "\nDivision durch Null, bei Cov[%d]",k);
	  continue;
	}
        cov[k][i][j] /= (classes[k][k] - 1.0);
	cov[k][j][i] = cov[k][i][j];
	cov[0][i][j] += (cov[k][i][j] * (classes[k][k] - 1.0));
      }
  for(i = 1; i < NrAttrs(ds->domain); i++)
    for(j = i; j < NrAttrs(ds->domain); j++) {
      cov[0][i][j] /= (ds->nr_exs - NR_CLASSES);
      cov[0][i][j] *= ((ds->nr_exs)-NR_CLASSES-1); /* NEW */
      cov[0][i][j] /= ((ds->nr_exs)-1);	/* NEW */
      cov[0][j][i] = cov[0][i][j];
      /* cov[NR_CLASSES+1][i][j] /= (ds->nr_exs - 1.0); */     /* NEW */      
      cov[NR_CLASSES+1][j][i] = cov[NR_CLASSES+1][i][j];
    }
    


  for(i = 1; i < NrAttrs(ds->domain); i++)
    for(j = 1; j < NrAttrs(ds->domain); j++)
      cov[0][i][j] = 1.0/(ds->nr_exs-1)* cov[NR_CLASSES+1][i][j];
 
  fflush (stdout);

  Show_Covar(ds->domain, cov);
  return cov;
}

double **
covariance1 (Dataset *ds, double ***momentos) {
  register int i, j, k;
  double x, y, mx, my;
  double **cov;
  int nr_cl;

  if ((cov = dmatrix(1, NrAttrs(ds->domain), 1, NrAttrs(ds->domain))) != NULL) {
    /* ************************************************
       Compute Class covariance matrices from Data
       *************************************************** */
    for (i = 1; i <= ds->nr_exs; i++) {
      for(j = 1; j <= NrAttrs(ds->domain); j++) {
	switch(TypeAttr(ds->domain, j)) {
	case continuous: 
	  x = CValAttEx(ds->instances[i], j); 
	  mx = momentos[j][0][3];
	  break;
	case ordered:
	case integer:
	case nominal:
	  x = (double) DValAttEx(ds->instances[i], j); 
	  mx = momentos[j][0][1];
	  break;
	case structured:	/* (TK) */
	  break;
	}
	for(k = j; k <= NrAttrs(ds->domain); k++) {
	  switch(TypeAttr(ds->domain, k)) {
	  case continuous: 
	    y = CValAttEx(ds->instances[i], k); 
	    my = momentos[k][0][3];
	    break;
	  case ordered:
	  case integer:
	  case nominal:
	    y = (double) DValAttEx(ds->instances[i], k); 
	    my = momentos[k][0][1];
	    break;
	  case structured:	/* (TK) */
	    break;
	  }
	  /*	fprintf (stdout, "Cov j %d k %d x %.5f mx %.5f y %.5f my %.5f\n", j, k, x, mx, y, my); */
	  cov[j][k] += (x - mx) * (y - my);
	}
      }
    }
    /* ************************************************
       Normalize matrices and 
       compute Pooled covariance Matrix
       *************************************************** */
    for(i = 1; i < NrAttrs(ds->domain); i++)
      for(j = i; j < NrAttrs(ds->domain); j++) {
	cov[i][j] /= (ds->nr_exs - 1.0);
	cov[j][i] = cov[i][j];
      }
    /* new */
  
    nr_cl = NValsAttr(ds -> domain,ds->domain->nr_attrs);  
    for(i = 1; i < NrAttrs(ds->domain); i++)
      for(j = 1; j < NrAttrs(ds->domain); j++) {
        cov[i][j] *= ((ds->nr_exs)-nr_cl-1);
        cov[i][j] /= ((ds->nr_exs)-1); 
      }
     
    /* end  */  
     
    fprintf (stdout, "Covariance Matrix\n");
    /*    ShowMatrix(cov, 1, NrAttrs(ds->domain)-1,  1, NrAttrs(ds->domain)-1); */ 
    ShowCovMatrix(cov, 1, NrAttrs(ds->domain)-1,  1, NrAttrs(ds->domain)-1);
    fflush (stdout);
  }
  fflush (stdout);
  return cov;
}

void 
Show_Covar(DomainInfo *names, double ***cov) {
  register int i;		/* (TK): unused: , j, k; */
  char *name;

  FILE *fp = NULL; /* TK: used for debug */

  VERBOSE(3) {
    for(i = 1; i <= NR_CLASSES; i++) {
      name = LblValId(names,names -> nr_attrs,i);
      fprintf (stdout, "\nCovariance Matrix of Class %d (%s)\n", i,name);
      ShowMatrix(cov[i], 1, NrAttrs(names)-1,  1, NrAttrs(names)-1);
      if (debug) {
	fp = DEBUGFP;
	fprintf (fp, "\n/* Covariance Matrix of Class %d (%s) */\n", i,name);
	fprintf (fp, "double cov_%s[%d][%d]\n", 
		 name, NrAttrs(names), NrAttrs(names));
	fclose (fp);
	PrintMatrix (cov[i], DEBUGFP, 1, NrAttrs(names)-1,  1, NrAttrs(names)-1);
      }
    }
  }
  VERBOSE(2) {
    fprintf (stdout, "\nPooled Covariance Matrix\n");
    ShowMatrix(cov[0], 1, NrAttrs(names)-1,  1, NrAttrs(names)-1);
   
    if (debug) {
      fp = DEBUGFP;
      fprintf (fp, "\n/* Pooled Covariance Matrix */\n");
      fprintf (fp, "double cov_pooled[%d][%d]\n", 
	       NrAttrs(names),  NrAttrs(names));
      fclose (fp);
      PrintMatrix(cov[0], DEBUGFP, 1, NrAttrs(names)-1,  1, NrAttrs(names)-1);
    }
    
    fprintf (stdout, "Total Covariance Matrix\n");
    ShowMatrix(cov[NR_CLASSES+1], 1, NrAttrs(names)-1,  1, NrAttrs(names)-1);

    if (debug) {
      fp = DEBUGFP;
      fprintf (fp, "/* Total Covariance Matrix */\n");
      fprintf (fp, "double cov_total[%d][%d]\n", 
	       NrAttrs(names),  NrAttrs(names));
      fclose (fp);
      PrintMatrix(cov[NR_CLASSES+1], DEBUGFP, 1, NrAttrs(names)-1,  1, NrAttrs(names)-1);
    }
  }

  fflush (stdout);
}

/***************************************/
/*  Principal Components Analysis      */
/***************************************/
double *
pca(DomainInfo *names, double ***cov, int nr_cl,int **dist) {
  register int i, j, k, nr;
  /* (TK): unused: int n, *perm; */
  /* (TK): unused: double sum = 0.0; */
  double **within, **between;	/* (TK): unused: , **p, **evect; */
  /* (TK): unused: double *ev; */
  /* (TK): unused: Eigenval *result; */
  int num;
  double **inv, **res, **index; /* (TK): unused: ,**neu; */
  double *eigen;

  nr = NrAttrs(names) - 1;
  if ((within = dmatrix(1,NrAttrs(names) - 1, 1, NrAttrs(names) - 1)) != NULL)
    if ((between = dmatrix(1, NrAttrs(names) - 1, 1, NrAttrs(names) - 1)) != 
	NULL) {
      for(i = 1; i <= NR_CLASSES; i++)
	for (j = 1; j < NrAttrs(names); j++) 
	  for (k = 1; k < NrAttrs(names); k++) {
	    num = dist[0][i];	/* NEW */
	    within[j][k] += ((num-1)*cov[i][j][k]); /* NEW */
	  }  
      for (j = 1; j < NrAttrs(names); j++) 
	for (k = 1; k < NrAttrs(names); k++) {
	  /*	  within[j][k] /= nr_cl;    */
	  between[j][k] = cov[NR_CLASSES + 1][j][k] - within[j][k];
	}
      VERBOSE(2) {
	print_line();
	fprintf (stdout, "\nBetween groups\n");
	ShowMatrix(between, 1, NrAttrs(names)-1,1, NrAttrs(names) -1); 
	fprintf (stdout, "\nWithin groups\n");
	ShowMatrix(within, 1, NrAttrs(names) -1,1, NrAttrs(names) -1); 
      }
      
      /* NEW  NEW  NEW */
       
      inv = matrix_mem(NrAttrs(names) -1,NrAttrs(names) -1);
      res = matrix_mem(NrAttrs(names) -1,NrAttrs(names) -1);
      index = matrix_mem(NrAttrs(names) -1,NrAttrs(names) -1);
     
      for (j = 0; j < NrAttrs(names) -1; j++) /* new index  */
	for (k = 0; k < NrAttrs(names)-1; k++) 
          index[j][k] = within[j+1][k+1];
     
      /* changed ABS to fabs */
      if((fabs(determinant(index,NrAttrs(names) -1))) < 0.0000000001) {
	fprintf (stdout, "\nNo computation of eigenvalues\n");
	fprintf (stdout, "\nReason: Determinant of W = 0");
	return NULL;
      }
       
      inv = inverse(index, NrAttrs(names) -1);        

      for (j = 0; j < NrAttrs(names) -1; j++) /* (W^-1) * B */
	for (k = 0; k < NrAttrs(names)-1; k++) {  
          for (i = 0; i < NrAttrs(names)-1; i++)
	    res[j][k] += inv[j][i] * between[i+1][k+1];
        } 
      /*
	 neu = matrix_mem(NrAttrs(names) -1,NrAttrs(names) -1);
	 for (j = 0; j < NrAttrs(names) -1; j++) 		
	 for (k = 0; k < NrAttrs(names)-1; k++) {  
	 for (i = 0; i < NrAttrs(names)-1; i++)
	 neu[j][k] += inv[j][i] * index[i][k];
	 } 
	 
	 
	 show_matrix(neu,NrAttrs(names) -1, NrAttrs(names) -1); 
	 
	 */
     
      print_line();
      fprintf (stdout, "\n#*** Eigenvalues ***\n");
      VERBOSE(2) {
	fprintf (stdout, "\n(Within)^(-1) * Between\n\n");
	ShowMatrix(res,0,NrAttrs(names)-2,0,NrAttrs(names)-2); 
	fprintf (stdout, "\n");

      }
      fflush (stdout);
/*       fprintf (stderr, "Eigenvalue dim: %d\n", NrAttrs(names)-1); */
/*       fflush (stderr); */
      eigen = Eigenvalue(res,NrAttrs(names)-1);

      fflush (stdout);
      fflush (stderr);
      
      free(res);
      free(index);
      free(inv); 
      
      /* END  END END */				/* changed by      */
      /* Ulrike Zintz    */
      /* date: 3.12.97   */
      
      /*	The computation of eigenvalues is correct only in the
		cases when the matrix W^(-1) * B is symmetrical 
		*/   	
      
      /* p = lu_pmi(within, between, NrAttrs(names) - 1);    
	 VERBOSE(2) {                                         
	 fprintf (stdout, "\nWi.B\n");                              
	 ShowMatrix(p, 1, NrAttrs(names) -1, 1, NrAttrs(names) -1);
	 }						
	 ev = dvector(1, NrAttrs(names) - 1);
	 evect = jacobi(p, NrAttrs(names) - 1, ev, &n);
	 
	 VERBOSE(2) {
	 fprintf (stdout, "\n");
	 for(i = 1; i < NrAttrs(names); i++)
	 fprintf (stdout, "Eigenvalue %d: %.5f\n", i, ev[i]);
	 }
	 VERBOSE(3) {
	 fprintf (stdout, "\n\nEigen Vectores\n\n");
	 ShowMatrix(evect, 1, NrAttrs(names) -1,1, NrAttrs(names) -1); 
	 }
	 if ((perm = ivector(1, NrAttrs(names) -1)) != NULL) {
	 sort_eigen(ev, evect, perm, NrAttrs(names)-1, 0);
	 nr = nr_cl > NrAttrs(names) -1 ? NrAttrs(names) -1 : nr_cl;
	 for(i = 1; i <= nr; i++) sum += ev[i] > 0.0 ? ev[i] : 0.0;
	 fprintf (stdout, "\nFract: %.5f \nCancor: %.5f\n",ev[1] / sum, sqrt(ev[1] / (1 + ev[1])));
	 }
	 result -> eig_vect = evect;  
	 result -> eig_val = ev;  
	 */
      return eigen;
    }
  return NULL;
}

/* (TK): changed from 'double **' to void */
void 
pca1(DomainInfo *names, double **cov) { 
  register int i; 
  double *ev, **evect; 
  int n; 

  ev = dvector(1, NrAttrs(names)); 
  if ((evect = jacobi(cov, NrAttrs(names), ev, &n)) != NULL) {
    VERBOSE(2) { 
      for(i = 1; i < NrAttrs(names); i++) 
	fprintf (stdout, "Eigenvalue %d: %.5f\n", i, ev[i]); 
    } 
    VERBOSE(3) { 
      fprintf (stdout, "\nEigen Vectores\n\n"); 
      ShowMatrix(evect, 1, NrAttrs(names)-1,1, NrAttrs(names)-1);  
    } 
    
    fflush (stdout);
  }

} 
/**************************************/
/*            S.D. Ratio              */
/**************************************/
double 
sd_ratio(DomainInfo *names, int **classes, double ***covar, long nr_exs) {
  register int i;
  double m = 0.0, lambda = 0.0;

  for(i = 1; i <= NR_CLASSES; i++) {
    m += (classes[i][i] - 1.0) * m_statistics(covar[i], covar[0], NrAttrs(names) 
					      -1);
    lambda += (1.0 / (classes[i][i] - 1.0)) - (1.0 / (nr_exs - NR_CLASSES));
  }
  lambda = 1.0 - ((2.0 *  SQR(NrAttrs(names) -1) + 3.0 * (NrAttrs(names) -1) - 
		   1.0) / (6.0 * (NrAttrs(names)) * (NR_CLASSES - 1.0))) * lambda;
  m *= lambda;
  return (exp((double) m / ((NrAttrs(names) -1) * (nr_exs - NR_CLASSES))));
}

static double 
m_statistics(double **si, double **s, int m) {
  double **p, d;

  p = cholpmi(si, s, m);
  d = lu_determinant(p, m);

  VERBOSE(3) {
    ShowMatrix(p, 1, m,1, m); 
    fprintf (stdout, "Determinante %.5f\n\n\n", d);
  }   
  fflush (stdout);

  free_dmatrix(p, 1, m, 1, m);
  
  return(log((double) fabs(d)));
}
/**************************************/
/*   Public Methods  Moments          */
/**************************************/
double ***
MyMoments(Dataset *ds, int goal, int Low, int High) {
  register int classe, valor, j, nr_classes;
  double cval, mean, std, aux;
  char dummy[128];
  unsigned long i;
  double ***stat;
  int *class_freq;
  
  nr_classes = NValsAttr(ds->domain, goal);
  stat = init_moments(ds->domain, nr_classes, NrAttrs(ds->domain));
  class_freq = ivector(0, nr_classes);
  for(i = Low; i <= High; i++) {
    if(nr_classes) {
      classe = DValAttEx(ds->instances[i], goal);
      if (TypeAttr(ds->domain, goal) == integer) {
        sprintf(dummy, "%d", classe);
        classe = IdValLbl(ds->domain, goal, dummy);
      }
      ++class_freq[classe]; 
    }
    ++class_freq[0];
    for(j = 1; j <= NrAttrs(ds->domain); j++) {
      if(NormalVal(ds->instances[i][j])) {
	switch(TypeAttr(ds->domain, j)) {
	case nominal:
	case ordered:
	case integer:
	  valor = DValAttEx(ds->instances[i], j);
	  aux = SQR(valor);
	  stat[j][0][1] += valor;
	  stat[j][0][2] += aux;
	  stat[j][0][3] += valor * aux;
	  stat[j][0][4] += SQR(aux);
	  if(nr_classes) {
	    stat[j][classe][1] += valor;
	    stat[j][classe][2] += aux;
	    stat[j][classe][3] += valor * aux;
	    stat[j][classe][4] += SQR(aux);
	  }
	  break;
	case continuous:
	  cval = CValAttEx(ds->instances[i], j);
	  aux = SQR(cval);
	  if(stat[j][0][1] > cval) stat[j][0][1] = cval;
	  if(stat[j][0][2] < cval) stat[j][0][2] = cval;
	  stat[j][0][3] += cval;
	  stat[j][0][4] += aux;
	  stat[j][0][5] += (aux * cval);
	  stat[j][0][6] += SQR(aux);
	  stat[j][0][7]++;
	  if(nr_classes) {
	    if(stat[j][classe][1] > cval) stat[j][classe][1] = cval;
	    if(stat[j][classe][2] < cval) stat[j][classe][2] = cval;
	    stat[j][classe][3] += cval;
	    stat[j][classe][4] += aux;
	    stat[j][classe][5] += (aux * cval);
	    stat[j][classe][6] += SQR(aux);
	    stat[j][classe][7]++;    
	  }
	  break;
	case structured:	/* (TK) */
	  break;
	}
      }
    }
  }
  for(j = 1; j <= NrAttrs(ds->domain); j++) {
    switch(TypeAttr(ds->domain, j)) {
    case nominal:
    case ordered:
    case integer:
      for(i = 0; i <= nr_classes; i++) {
	mean =  stat[j][i][1] / (double) class_freq[i];
	std = STD(stat[j][i][1],stat[j][i][2],(double) class_freq[i]);
	stat[j][i][4] = KURT(stat[j][i][4],stat[j][i][3], 
			     stat[j][i][2],stat[j][i][1], mean, std, (double) class_freq[i]);
	stat[j][i][3] = SKEW(stat[j][i][3], stat[j][i][2],stat[j][i][1], mean, 
			     std, (double) class_freq[i]);
        stat[j][i][2] = std;
        stat[j][i][1] = mean;
      }
      break; 
    case continuous:
      for(i = 0; i <= nr_classes; i++) {
	mean = stat[j][i][3] / (double) class_freq[i];
	std = STD(stat[j][i][3],stat[j][i][4],(double) class_freq[i]);
	stat[j][i][6] = KURT(stat[j][i][6],stat[j][i][5], 
			     stat[j][i][4],stat[j][i][3], mean, std, (double) class_freq[i]);
	stat[j][i][5] = SKEW(stat[j][i][5], stat[j][i][4],stat[j][i][3], mean, 
			     std, (double) class_freq[i]);
        stat[j][i][4] = std;
        stat[j][i][3] = mean;
      }
      break;
    case structured:		/* (TK) */
      break;
    }
  }
  return stat;
} 

void 
MyShowMoments(DomainInfo *domain, double ***moments, int goal) 
{
  register int j, i, /*n,*/ nr_classes, ncont = 0; /* (TK): unused: k */
  double mskew = 0.0, mkurt =0.0; 
  /* (TK): unused: double *spannw; */
  /* (TK): unused: double sum = 0, min, max, avg; */
  double conf_min=0.0, conf_max=0.0;	/* TK */
  char ClassName[255];	
  int l,AttrNum;

  /* TK: changed ABS to fabs */

  nr_classes = NValsAttr(domain, goal);
  fprintf (stdout, "\n#*** Attribute Statistics ***\n");
  for(j = 1; j <= NrAttrs(domain)-1; j++) {
    	
 	 AttrNum=GlobalNum(domain,j);	

	 fprintf (stdout, "Attr_Name_%d: %s\n",AttrNum, NameAttr(domain,j));
	 fprintf (stdout, "Attr_Type_%d: continuous\n",AttrNum);

	//fprintf (stdout, "\nAttribute: %s %s\n", NameAttr(domain,j), j == goal ? "[TARGET]" : " ");      
    switch(TypeAttr(domain, j)) {
    case nominal:
    case ordered:
    case integer:
      fprintf (stdout, "\tMean\tStd.Dev\tSkew\tKurt\n");
	  for(i = 0; i <= nr_classes; i++) {
        if (i) {
	  fprintf (stdout, "%.7s\t", LblValId(domain, goal, i));
	  if (moments[j][i][2]) {
	    mskew += fabs(moments[j][i][3]);
	    mkurt += fabs(moments[j][i][4]);
	    ++ncont;
	  }
	}
        else {
	  fprintf (stdout, "DS:\t");
	  if (!nr_classes) {
	    if (moments[j][i][2]) {
	      mskew += fabs(moments[j][i][3]);
	      mkurt += fabs(moments[j][i][4]);
	      ++ncont;
	    }
	  }
	}
        fprintf (stdout, "%9.5f\t", moments[j][i][1]);
        fprintf (stdout, "%9.5f\t", moments[j][i][2]);
        fprintf (stdout, "%9.5f\t", moments[j][i][3]);
        fprintf (stdout, "%9.5f\n", moments[j][i][4]);
      }
      break;
    case continuous:
 
		for(i = 0; i <= nr_classes; i++) {
		 
        if (i) {
           sprintf(ClassName,"Cl%d",i);
          //fprintf (stdout, "%.8s ", LblValId(domain, goal, i));
	  if (strlen(LblValId(domain,goal,i)) < 8) {
	    for (l=0; l<8-strlen(LblValId(domain,goal,i)); l++) {
	      //fprintf (stdout, " ");
	    }
	  }
//	  n = moments[goal][i][i];
	  if (moments[j][i][4]) {
	    if ((moments[j][i][5]==moments[j][i][5]) && (moments[j][i][6]==moments[j][i][6])) {

	      /* added second if-loop to avoid NaN in MeanSkew when NaN in Skew_Cl_Attr appear */

	      mskew += fabs(moments[j][i][5]);
	      mkurt += fabs(moments[j][i][6]);
	      ++ncont;   
	    }
	  }
        }
        else  {
	  //fprintf (stdout, "DS:      ");
	  sprintf(ClassName,"All");
	  if (!nr_classes) {
	    if (moments[j][i][4]) {
	      mskew += fabs(moments[j][i][5]);
	      mkurt += fabs(moments[j][i][6]);
	      ++ncont; 	
	    } 
	  }
	}
	
	fprintf (stdout, "Attr_NrExamples_%s_%d: %-10d\n",ClassName, AttrNum, (int)(moments[j][i][7]));
		
	if (moments[j][i][7] == 0) {
	  fprintf (stdout, "Attr_Min_%s_%d: *\n", ClassName, AttrNum);
	  fprintf (stdout, "Attr_Max_%s_%d: *\n", ClassName, AttrNum);
	  fprintf (stdout, "Attr_Mean_%s_%d: *\n", ClassName, AttrNum);
	  fprintf (stdout, "Attr_StdDev_%s_%d: *\n", ClassName, AttrNum);
	  fprintf (stdout, "Attr_Skew_%s_%d: *", ClassName, AttrNum);
	  fprintf (stdout, "Attr_Kurt_%s_%d: *", ClassName, AttrNum);
	  fprintf (stdout, "Attr_Range_%s_%d: *\n", ClassName,AttrNum);
	}
	else if (moments[j][i][7] == 1) {
	  fprintf (stdout, "Attr_Min_%s_%d: %-10.3f\n", ClassName, AttrNum,moments[j][i][1]);
	  fprintf (stdout, "Attr_Max_%s_%d: %-10.3f\n", ClassName, AttrNum, moments[j][i][2]);
	  fprintf (stdout, "Attr_Mean_%s_%d: %-10.3f\n", ClassName, AttrNum,moments[j][i][3]);
	  fprintf (stdout, "Attr_StdDev_%s_%d: %-10.3f\n", ClassName, AttrNum,moments[j][i][4]);
	  fprintf (stdout, "Attr_Skew_%s_%d: *", ClassName, AttrNum);
	  fprintf (stdout, "Attr_Kurt_%s_%d: *", ClassName, AttrNum);
	  fprintf (stdout, "Attr_Range_%s_%d: %-10.3f\n", ClassName, AttrNum,moments[j][i][2]-moments[j][i][1]);
	}
	else { 
	  fprintf (stdout, "Attr_Min_%s_%d: %-10.3f\n", ClassName, AttrNum,moments[j][i][1]);
	  fprintf (stdout, "Attr_Max_%s_%d: %-10.3f\n", ClassName, AttrNum, moments[j][i][2]);
	  fprintf (stdout, "Attr_Mean_%s_%d: %-10.3f\n", ClassName, AttrNum,moments[j][i][3]);
	  fprintf (stdout, "Attr_StdDev_%s_%d: %-10.3f\n", ClassName, AttrNum,moments[j][i][4]);
	  fprintf (stdout, "Attr_Skew_%s_%d: %-10.3f\n", ClassName, AttrNum,moments[j][i][5]);
	  fprintf (stdout, "Attr_Kurt_%s_%d: %-10.3f\n", ClassName, AttrNum,moments[j][i][6]);
	  fprintf (stdout, "Attr_Range_%s_%d: %-10.3f\n", ClassName, AttrNum,moments[j][i][2]-moments[j][i][1]);
	}
      }
      break;
    case structured:		/* (TK) */
      break;
    }
  }

  fprintf (stdout, "\nMean_Absolute_Skew:     %8.5f\n", mskew/ncont);
  fprintf (stdout,   "MeanKurtosis: %8.5f\n", mkurt/ncont);
 
  fflush (stdout);
  
  results[MEAN_SKEW] =  mskew / ncont;
  results[MEAN_KURTOSIS] =  mkurt / ncont;

  /* TK: */
  VERBOSE(3) {
    print_line ();
    fprintf (stdout, "Confidence Intervals for the mean\n\n");
    for (i=1; i<=NrAttrs(domain)-1; i++) {
      if ((TypeAttr(domain, i)) == continuous) {
	fprintf (stdout, "\nAttribute: %s\n", domain->attrs[i].name);
	fprintf (stdout, "                 mean     stddev         interval         alpha     nr_exs\n");
	for (j=0; j<=nr_classes; j++) {

	  if (j==0) {
	    fprintf (stdout, "DS:         ");
	    fflush (stdout);
	    mean_confidence_interval (moments[i][j][3], moments[i][j][4], 
				      moments[i][0][7], CONF_ALPHA, 
				      &conf_min, &conf_max);
	  }
	  else {
	    mean_confidence_interval (moments[i][j][3], moments[i][j][4], 
				      moments[i][j][7], CONF_ALPHA, 
				      &conf_min, &conf_max);
	    fprintf (stdout, "%.10s: ", LblValId(domain, goal, j));
	    if (strlen(LblValId(domain, goal, j)) < 10) {
	      for (l=0; l<10-strlen(LblValId(domain, goal, j)); l++) {
		fprintf (stdout, " ");
	      }
	    }
	  }
	  if (j==0) {
	    fprintf (stdout, "%9.5f  %9.5f  [%9.5f,%9.5f]  (%1.3f)  %8d\n", 
		     moments[i][j][3], moments[i][j][4], conf_min, conf_max, 
		     CONF_ALPHA, (int) moments[i][0][7]);
	    fflush (stdout);
	  }
	  else {
	    fprintf (stdout, "%9.5f  %9.5f  [%9.5f,%9.5f]  (%1.3f)  %8d\n", 
		     moments[i][j][3], moments[i][j][4], conf_min, conf_max, 
		     CONF_ALPHA, (int) moments[i][j][7]);
	    fflush (stdout);
	  }
	}
      }
    }
    
    fflush (stdout);
  }
  /* TK end. */
  
}


//***************************************OLD VERSION*******************************************/
void 
MyShowMoments_old(DomainInfo *domain, double ***moments, int goal) 
{
  register int j, i, n, nr_classes, ncont = 0; /* (TK): unused: k */
  double mskew = 0.0, mkurt =0.0;
  /* (TK): unused: double *spannw; */
  /* (TK): unused: double sum = 0, min, max, avg; */
  double conf_min=0.0, conf_max=0.0;	/* TK */

  int l;

  /* TK: changed ABS to fabs */

  nr_classes = NValsAttr(domain, goal);
  fprintf (stdout, "\n#*** Attribute Statistics ***\n");
  for(j = 1; j <= NrAttrs(domain)-1; j++) {
    fprintf (stdout, "\nAttribute: %s %s\n", NameAttr(domain,j), j == goal ? "[TARGET]" : " ");      
    switch(TypeAttr(domain, j)) {
    case nominal:
    case ordered:
    case integer:
      fprintf (stdout, "\tMean\tStd.Dev\tSkew\tKurt\n");
      for(i = 0; i <= nr_classes; i++) {
        if (i) {
	  fprintf (stdout, "%.7s\t", LblValId(domain, goal, i));
	  if (moments[j][i][2]) {
	    mskew += fabs(moments[j][i][3]);
	    mkurt += fabs(moments[j][i][4]);
	    ++ncont;
	  }
	}
        else {
	  fprintf (stdout, "DS:\t");
	  if (!nr_classes) {
	    if (moments[j][i][2]) {
	      mskew += fabs(moments[j][i][3]);
	      mkurt += fabs(moments[j][i][4]);
	      ++ncont;
	    }
	  }
	}
        fprintf (stdout, "%9.5f\t", moments[j][i][1]);
        fprintf (stdout, "%9.5f\t", moments[j][i][2]);
        fprintf (stdout, "%9.5f\t", moments[j][i][3]);
        fprintf (stdout, "%9.5f\n", moments[j][i][4]);
      }
      break;
    case continuous:
/*       fprintf (stdout, "\t\t   Min.\t\tMax.\t  Mean\t\tStd.Dev\t\tSkew\t  Kurt\t  Spannw.\n"); */
      fprintf (stdout, "         %6s %10s  %10s  %10s  %10s  %10s  %10s  %10s\n", 
	       "nr_exs","Min","Max", "Mean", "StdDev", "Skew", "Kurt", "Range");
      for(i = 0; i <= nr_classes; i++) {        
        if (i) {
          fprintf (stdout, "%.8s ", LblValId(domain, goal, i));
	  if (strlen(LblValId(domain,goal,i)) < 8) {
	    for (l=0; l<8-strlen(LblValId(domain,goal,i)); l++) {
	      fprintf (stdout, " ");
	    }
	  }
	  n = moments[goal][i][i];
	  if (moments[j][i][4]) {
	    mskew += fabs(moments[j][i][5]);
	    mkurt += fabs(moments[j][i][6]);
	    ++ncont;
	  }
        }
        else  {
	  fprintf (stdout, "DS:      ");
	  if (!nr_classes) {
	    if (moments[j][i][4]) {
	      mskew += fabs(moments[j][i][5]);
	      mkurt += fabs(moments[j][i][6]);
	      ++ncont;
	    }
	  }
	}
	if (moments[j][i][7] == 0) {
	  fprintf (stdout, "%6d  ", (int) (moments[j][i][7]));
	  fprintf (stdout, "%10s  ", "*");
	  fprintf (stdout, "%10s  ", "*");
	  fprintf (stdout, "%10s  ", "*");
	  fprintf (stdout, "%10s  ", "*");
	  fprintf (stdout, "%10s  ", "*");
	  fprintf (stdout, "%10s  ", "*");
	  fprintf (stdout, "%10s\n", "*");
	}
	else if (moments[j][i][7] == 1) {
	  fprintf (stdout, "%6d  ", (int) (moments[j][i][7]));
	  fprintf (stdout, "%10.3f  ", moments[j][i][1]);
	  fprintf (stdout, "%10.3f  ", moments[j][i][2]);
	  fprintf (stdout, "%10.3f  ", moments[j][i][3]);
	  fprintf (stdout, "%10.3f  ", moments[j][i][4]);
	  fprintf (stdout, "%10s  ", "*");
	  fprintf (stdout, "%10s  ", "*");
	  fprintf (stdout, "%10.3f\n", moments[j][i][2]-moments[j][i][1]);
	}
	else { 
	  fprintf (stdout, "%6d  ", (int) (moments[j][i][7]));
	  fprintf (stdout, "%10.3f  ", moments[j][i][1]);
	  fprintf (stdout, "%10.3f  ", moments[j][i][2]);
	  fprintf (stdout, "%10.3f  ", moments[j][i][3]);
	  fprintf (stdout, "%10.3f  ", moments[j][i][4]);
	  fprintf (stdout, "%10.3f  ", moments[j][i][5]);
	  fprintf (stdout, "%10.3f  ", moments[j][i][6]);
	  fprintf (stdout, "%10.3f\n", moments[j][i][2]-moments[j][i][1]);
	}
      }
      break;
    case structured:		/* (TK) */
      break;
    }
  }
  fprintf (stdout, "\nMean Skew:     %8.5f\n", mskew/ncont);
  fprintf (stdout,   "Mean Kurtosis: %8.5f\n", mkurt/ncont);
  fflush (stdout);
  
  results[MEAN_SKEW] =  mskew / ncont;
  results[MEAN_KURTOSIS] =  mkurt / ncont;

  /* TK: */
  VERBOSE(3) {
    print_line ();
    fprintf (stdout, "Confidence Intervals for the mean\n\n");
    for (i=1; i<=NrAttrs(domain)-1; i++) {
      if ((TypeAttr(domain, i)) == continuous) {
	fprintf (stdout, "\nAttribute: %s\n", domain->attrs[i].name);
	fprintf (stdout, "                 mean     stddev         interval         alpha     nr_exs\n");
	for (j=0; j<=nr_classes; j++) {

	  if (j==0) {
	    fprintf (stdout, "DS:         ");
	    fflush (stdout);
	    mean_confidence_interval (moments[i][j][3], moments[i][j][4], 
				      moments[i][0][7], CONF_ALPHA, 
				      &conf_min, &conf_max);
	  }
	  else {
	    mean_confidence_interval (moments[i][j][3], moments[i][j][4], 
				      moments[i][j][7], CONF_ALPHA, 
				      &conf_min, &conf_max);
	    fprintf (stdout, "%.10s: ", LblValId(domain, goal, j));
	    if (strlen(LblValId(domain, goal, j)) < 10) {
	      for (l=0; l<10-strlen(LblValId(domain, goal, j)); l++) {
		fprintf (stdout, " ");
	      }
	    }
	  }
	  if (j==0) {
	    fprintf (stdout, "%9.5f  %9.5f  [%9.5f,%9.5f]  (%1.3f)  %8d\n", 
		     moments[i][j][3], moments[i][j][4], conf_min, conf_max, 
		     CONF_ALPHA, (int) moments[i][0][7]);
	    fflush (stdout);
	  }
	  else {
	    fprintf (stdout, "%9.5f  %9.5f  [%9.5f,%9.5f]  (%1.3f)  %8d\n", 
		     moments[i][j][3], moments[i][j][4], conf_min, conf_max, 
		     CONF_ALPHA, (int) moments[i][j][7]);
	    fflush (stdout);
	  }
	}
      }
    }
    
    fflush (stdout);
  }
  /* TK end. */
  
}


/**************************************/
/*   Private Methods  Moments         */
/**************************************/
static double ***
init_moments(DomainInfo *domain, int nr_cl, int nr_at) {
  register int i, j, c = 0;
  double ***stat, **att;	/* (TK): unused: , *means; */

  if (nr_cl > 1)  c = nr_cl + 1;
  stat = (double ***) malloc((int) nr_at * sizeof(double **));  
  --stat;
  for(i = 1; i <= nr_at; i++) {
    switch(TypeAttr(domain, i)) {
    case nominal:
    case ordered:
    case integer:
      att = dmatrix(0, c, 0, 4);
      break;
    case continuous:
      att = dmatrix(0, c, 1, 7); /* expanded from 6 to 7 (TK) */
      for(j = 0; j <= c; j++){
        att[j][1] = DBL_MAX;  /* US changed FLT_MAX */
        att[j][2] = (double) -1.0 * DBL_MAX;  /* US changed FLT_MIN */
	att[j][3] = 0;
	att[j][4] = 0;
	att[j][5] = 0;
	att[j][6] = 0;
	att[j][7] = 0;
      }
      break;
    case structured:		/* (TK) */
      break;
    }
    stat[i] = att;
  }
  return stat;
}










