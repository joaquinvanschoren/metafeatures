/*
 * $Source /user1/grimmer/clem/src/achim/src/RCS/estatistica.h,v $
 * $Author: johann $
 * $Date: 2002/06/30 18:02:16 $
 * $Revision: 1.1.1.1 $
 */
/*
 *  $Log: estatistica.h,v $
 *  Revision 1.1.1.1  2002/06/30 18:02:16  johann
 *  Initial CVS version
 *
 *  Revision 4.1  1998/07/17 11:53:15  kleiner
 *  switching to revision 4.1
 *
 * Revision 3.3  1998/07/02  08:24:23  spuhl
 * *** empty log message ***
 *
 * Revision 3.2  1998/06/24  08:44:43  spuhl
 * changed MyShowMoments
 *
 * Revision 3.1  1998/06/23  12:41:31  kleiner
 * *** empty log message ***
 *
 * Revision 2.4  1998/06/04  10:49:44  kleiner
 * changed MyShowMoments (confidence intervals for mean)
 *
 * Revision 2.3  1998/05/18  07:03:59  kleiner
 * *** empty log message ***
 *
 * Revision 2.2  1998/05/17  17:39:11  Administrator
 * *** empty log message ***
 *
 *  Revision 2.1  1998/05/15 07:13:36  kleiner
 *  *** empty log message ***
 *
 * Revision 1.2  1998/05/07  16:19:00  kleiner
 * *** empty log message ***
 *
 */

/*  Statistical Methods     FILE:estatistica.h */

double ***covariance(Dataset *ds, int goal, double ***dist, int **classes);
void Show_Covar(DomainInfo *names, double ***cov);
void Show_Correl(double **m, int n);
double **correlacoes(Dataset *ds, int goal, double ***momentos, double ***covar);
double *pca(DomainInfo *names, double ***cov, int nr_cl,int **dist);
void MyShowMoments(DomainInfo *domain, double ***moments, int goal);
double ***MyMoments(Dataset *ds, int goal, int Low, int High);
double sd_ratio(DomainInfo *domain, int **classes, double ***covar, long 
nr_exs);
double **covariance1(Dataset *ds, double ***momentos);
double **correlacao(Dataset *ds, double ***momentos, double **covar);
/* (TK): changed from 'double **' to void */
void pca1(DomainInfo *names, double **cov);
