/*
 * $Source /user1/grimmer/clem/src/achim/src/RCS/measures.h,v $
 * $Author: johann $
 * $Date: 2002/06/30 18:02:16 $
 * $Revision: 1.1.1.1 $
 */
/*
 *  $Log: measures.h,v $
 *  Revision 1.1.1.1  2002/06/30 18:02:16  johann
 *  Initial CVS version
 *
 *  Revision 4.1  1998/07/17 11:53:57  kleiner
 *  switching to revision 4.1
 *
 * Revision 3.4  1998/07/07  16:45:17  spuhl
 * changed sort-functions to fix qsort warning
 *
 * Revision 3.3  1998/07/02  08:24:50  spuhl
 * *** empty log message ***
 *
 * Revision 3.2  1998/06/29  06:41:50  spuhl
 * show_crit_value replaced by calc_crit_value
 *
 * Revision 3.1  1998/06/23  12:41:48  kleiner
 * *** empty log message ***
 *
 * Revision 2.5  1998/06/04  10:50:29  kleiner
 * *** empty log message ***
 *
 * Revision 2.4  1998/05/18  07:04:18  kleiner
 * *** empty log message ***
 *
 * Revision 2.3  1998/05/17  17:39:22  Administrator
 * *** empty log message ***
 *
 *  Revision 2.2  1998/05/15 14:23:36  kleiner
 *  *** empty log message ***
 *
 * Revision 2.1  1998/05/15  07:13:54  kleiner
 * *** empty log message ***
 *
 * Revision 1.3  1998/05/13  13:46:01  kleiner
 * version 2.2
 *
 * Revision 1.2  1998/05/12  14:58:09  kleiner
 * added output
 *
 */

/*           Statistical Methods        */
/*            File: measures.h          */                 


/*    definitions  */

/*# define ABS(a)    (a >= 0) ? a:(-a) 
# define max(a,b)  (a >= b) ? a : b;
# define min(a,b)  (a < b)  ? a : b; (TK): unused */

#ifndef quad
# define quad(a)   (a*a)
#endif
# define qquad(a)  quad(quad(a))
# define qqquad(a) quad(qquad(a))


/*   typedefinitions   */  

typedef struct vect_val {
  double **eig_vect;
  double *eig_val;
} Eigenval;


typedef struct numeric_inst {
  double **inst;
  int *nr_ex;
} NumericInst;



/*    declarations  */

/* TK */ 
void mean_confidence_interval (double M, double S, unsigned long N, 
			       double Conf, double *Mmin, double *Mmax);

void show_lage_streu(Dataset *ds);
double emp_quant(double *vector, long int nr_exs, double p); 
double quar_dist(double *vector, long int nr_exs, double low, double high);
double med_std_dev(double *vector, long int nr_exs); 
double alpha_getr(double *vector, long int nr_exs, double alpha);
void show_multi_cor(double **matrix, int dim, Dataset *ds);
void show_bhep_test(double **matrix, int rows, int cols, double **s_n, double beta);
/* void show_crit_value(double beta, double p, double value);  US replaced */
double calc_crit_value(double beta, double p, double value);
void show_v_stat(double *eig_vector, Dataset *ds);
double wilk_lambda(double *s_eig_vec, int nr_eig);
double bartlett_stat(double *s_eig_vec, int nr_eig, int nr_attr, int nr_cl, int nr_ex);
double eigen_val(double value, int nr_attr, int nr_cl, int nr_ex);
DomainInfo *make_domain(DomainInfo *domain, int nr_attr);
AttrVal ***get_instances(Dataset *ds, int nr_num, int nr_sym);
void show_gini(int ***dist, Dataset *ds);
void show_relevanz(int ***dist, Dataset *ds);
void show_g_func(int ***dist, Dataset *ds);
void ShowMStat(Dataset *ds, double ***cov, int **dist);
double *Eigenvalue(double **mat, int dim);
double Fract(double *eigval, int nr);
double Cancor(double maxeig);
double std_dev(double *vector, int nr_ex, int low, int high);
double alpha_stddev(double *temp,int nr_ex);
double standarddev(double *vector,int nr_ex);



/*************Wojciech***Kwedlo****************/


void matrix_free(double **p);
void vector_free(double *p);
/*************Wojciech***Kwedlo**(end)*********/

/*   help-functions, help-procedures       */

double **matrix_mem(int rows, int cols);
double *vector_mem(int dim);
double determinant(double **matrix, int dim); 
double **inverse(double **matrix, int dim);
NumericInst *read_numeric(Dataset *ds);
double **read_symbolic(Dataset *ds);
int compare_double (const void *i, const void *j); /* TK: added !!! */
int compare_chars(const void *s1, const void *s2); /* US added */
int compare_attrval (const void *i, const void *j); /* US added */
/* TK (no longer used): void sort_vector(double *vector, int low, int high); */
/* TK (not used): double **sort_matrix(double **matrix, int rows, int cols);*/
void swap(double *vector, int i, int j);
void show_matrix(double **matrix, int rows, int cols);
void show_vector(double *vector, int low, int high);
/* TK: added paramter det */
double bhep_factor(double **matrix, int rows, int cols, double **s_n, double beta, double det);
double diff_norm(double **matrix, int rows, int cols, double **inv, double beta);
double euklid_norm(double *vector, double **inverse, int dim);
double norm(double **matrix, int rows, int cols, double **inv, double beta);
double **bhep_matrix(double **matrix, int dim);
double **move_index(double **matrix, int rows, int cols);
double mue(double beta, double p);
double sigma(double beta, double p);
double phi_func(double alpha);
void print_line();
unsigned long int fac(int n);
double log_fac(int n);
double **get_mat(double **matrix, int index, int dim);
double *get_vec(double **matrix, int index, int dim);
double **read_numeric_old(Dataset *ds);
void NumSym(Dataset *ds, DomainInfo *names, Dataset *num, Dataset *sym);
void show_values(double *values, int nr, Dataset *ds);
void ShowStat(Dataset *sym, Dataset *num, int ***dist);
double gamma_(int nr_num, int nr_cl, int **dist, int nr_ex);
double Sd_ratio(double m, int nr_num, int nr_cl, int **dist);
double **QR(double **mat, int dim);
double **MultMatQ(double **mat1, double **mat2, int dim);
char categ_of_examples(int nr_ex);
double **Transp(double **matrix, int dim);

















































































































