/*
 * $Source /user1/grimmer/clem/src/achim/src/RCS/framework.h,v $
 * $Author: johann $
 * $Date: 2002/06/30 18:02:16 $
 * $Revision: 1.1.1.1 $
 */
/*
 * Revision 5.1 2001/02/23 17:22:10 koepf
 * added ShowCovMatrix
 *
 *  $Log: framework.h,v $
 *  Revision 1.1.1.1  2002/06/30 18:02:16  johann
 *  Initial CVS version
 *
 *  Revision 4.1  1998/07/17 11:53:18  kleiner
 *  switching to revision 4.1
 *
 * Revision 3.4  1998/07/15  08:15:26  spuhl
 * avoided recursion
 *
 * Revision 3.3  1998/07/02  17:03:12  spuhl
 * added ShowTarget
 *
 * Revision 3.2  1998/07/02  08:24:25  spuhl
 * *** empty log message ***
 *
 * Revision 3.1  1998/06/23  12:41:32  kleiner
 * *** empty log message ***
 *
 * Revision 2.10  1998/06/12  12:25:25  thor
 * changed ShowDistribution
 *
 *  Revision 2.9  1998/06/06 17:07:05  thor
 *  added 'nr_unknown' to structure
 *
 *  Revision 2.8  1998/06/04 16:15:41  kleiner
 *  'isint'
 *
 * Revision 2.7  1998/06/04  10:50:00  kleiner
 * *** empty log message ***
 *
 * Revision 2.6  1998/05/27  14:07:19  kleiner
 * added print_matrix and debug
 *
 * Revision 2.5  1998/05/20  10:46:12  kleiner
 * *** empty log message ***
 *
 * Revision 2.4  1998/05/19  15:28:53  kleiner
 * strtol
 *
 * Revision 2.3  1998/05/18  07:04:00  kleiner
 * *** empty log message ***
 *
 * Revision 2.2  1998/05/17  17:39:13  Administrator
 * *** empty log message ***
 *
 *  Revision 2.1  1998/05/15 07:13:37  kleiner
 *  *** empty log message ***
 *
 * Revision 1.4  1998/05/14  16:40:18  kleiner
 * *** empty log message ***
 *
 * Revision 1.3  1998/05/13  13:45:35  kleiner
 * version 2.2
 *
 * Revision 1.2  1998/05/07  16:19:01  kleiner
 * *** empty log message ***
 *
 */

/*********************************************************************/
/*        FRAMEWORK FOR PROPOSITIONAL LEARNING                       */
/*        Versao 1.0 (10/12/1996)    FILE:framework.h                */
/*  Developed by: Joao Gama, Luis Torgo                              */
/*                LIACC - Uni.do Porto                               */
/*                {jgama, ltorgo}@ncc.up.pt                          */
/*********************************************************************/
#ifndef _FRAMEWORK_
#define _FRAMEWORK_

#define TRUE               1
#define FALSE              0

#define _DOMAIN_FILE_EXT   ".domain"

/* ======================================================================
   DATA TYPES 
   ---------------------------------------------------------------------- */
#define ContType        double    /* float */
#define Instance        AttrVal *

enum ValTypes { normal,unknown,dontcare };
enum AttrTypes { nominal,ordered,integer,continuous,structured };

union NormalVal {
  int               d;
  ContType          c;
};

typedef struct attr_val {
  enum  ValTypes    type_of_val;
  union NormalVal   val;
} AttrVal;


typedef struct attr_info {
  int				global_num;	/* Wojciech Kwedlo */
  char              *name;
  enum AttrTypes    type_attr;
  int		    isint;  /* TK */
  int	            nr_unknown; /* TK */
  unsigned int      nr_vals;
  void              *vals;
} AttrInfo;


typedef struct domain_info {
  char              *root_name;
  unsigned int      nr_attrs;
  AttrInfo          *attrs;
} DomainInfo;                                


typedef struct _ds{
  DomainInfo *domain;
  AttrVal **instances;
  unsigned long nr_exs;
}Dataset;

typedef struct cv_res {
  unsigned int      nfolds;
  double            avg,  std,  max,  min;
  double            *errors;
} CVResults;


typedef enum error_s { mad, mse, error_rate } ErrorStat;

extern int REGRESSION;        /* introduced for regression data sets, CK */
extern int ONLY_NUMERIC;      /* introduced to avoid the printout of information theoretic measures, CK */

/* ======================================================================
   DATA STRUCTURES 
   ---------------------------------------------------------------------- */

/* ================================================================= */
/* DATA STRUCTURES DEFINITIONS                                       */
/* ================================================================= */

/* ***************************************************************** */
/* The list definition                                               */
/* ***************************************************************** */
struct list {
  void          *data;
  struct list   *next;
};
typedef struct list List;


/* ================================================================= */
/* Macros                                                            */
/* ================================================================= */
#define empty(list)	        ((list) ? 0 : 1) 
#define head(list)		(list->data)
#define tail(list)		(list->next)

/* ================================================================= */
/* Functions Prototypes                                              */
/* ================================================================= */

/* Lists */
List *insert(void *, List *);
List *insert_end(void *, List *);
List *insert_ord(void *e,List *lst, int(*f)(void *e1, void *e2));
List *belongs(void *e,List *lst, int (*func)(void *e1, void *e2));
List *nth(unsigned long n,List *lst);
List *position(void *e,List *lst,int (*func)(void *e1, void *e2),unsigned long 
*nr);
List *delete(void *e,List *lst,int (*func)(void *e1, void *e2));
List *remove_nth(unsigned long int, List *, List **);
List *lstcat(List *lst1, List *lst2);
List *change_head(void *e, List *lst);

unsigned long count(List *lst);
List *last(List *lst);
void clean(List *lst);
void clean_all(List *lst);

void mapp(List *lst, void (* func) (void *e));
void mapp_inv(List *lst, void (* func) (void *e));

/* TK: not needed in here; moved to data_structures.c */
/* List *quick_sort(List *lst,int (* func)(void *e1, void *e2)); */

void print_list(List *lst,void (* func) (void *e));

/* ======================================================================
   S T R I N G S
   ---------------------------------------------------------------------- */

#define IS_IN(s1,s2) (strstr(s1,s2) != NULL)
#define EMPTY_STR(s) (*s == NULL)
#define EQUAL_STRS(s1,s2) (!strcmp(s1,s2))

#define v_split(str,sep,nwrds) _v_split(str, sep, nwrds, 1)

char *new_strcat(char *,char *);
char *new_nstrcat(int, ...);
char *new_strcpy(char *);

int is_a_int(char *);
int is_a_str(char *);
int is_a_real(char *);

int starts_by(char *,char *);

int eq_numstr(char *,char *);
int gt_numstr(char *,char *);
int lt_numstr(char *,char *);

int strgt(char *,char *);
int strlt(char *,char *);

char **a_str(int n, ...);

List *split(char *str,char *split_chars);
int contained(char,char *);
char **_v_split(char *, char *, int *, int );
char *ReadWord(char **, char *); /* used to be static */

/* ======================================================================
   F I L E S 
   ---------------------------------------------------------------------- */
#ifndef _FILES_HEADER_
#define _FILES_HEADER_

#define _MAX_WORD_LEN_  512  /* 256 */
#define _MAX_SIZE_LINE_ 4096  /* 256 */

#define TRUE 1

#define DIGIT(ch) (ch >= '0' && ch <= '9')
#define end_of_record(ch) ((ch == EOF) || (ch == '\n'))
#define _FIELD_SEP   ",\t:; "

/* US unused
#define ReadLine(fi, nrw) _ReadLine(fi, nrw, 1, _FIELD_SEP)
#define SReadLine(fi, nrw, f_sep) _ReadLine(fi, nrw, 1, f_sep)
changed
#define ReadFile(fi, nrl, nrw) _ReadFile(fi, nrl, nrw, 1, _FIELD_SEP)
#define SReadFile(fi, nrl, nrw, f_sep) _ReadFile(fi, nrl, nrw, 1, f_sep)
*/

/* US no longer used
#define ReadFile(fi, nrl, nrw) _ReadFile(fi, nrl, nrw, _FIELD_SEP)
#define SReadFile(fi, nrl, nrw, f_sep) _ReadFile(fi, nrl, nrw, f_sep)
*/

/* US unused functions
char *read_word(FILE *f, char *sep_word);
int read_int(FILE *f);
double read_float(FILE *f);
void skip_line(FILE *f);
unsigned long nr_lines(char *);
char *read_line(FILE *);
List *read_text_lines(FILE *, unsigned long *nlin);
int existing_file(char *);
*/

/* US changed
char ***_ReadFile(FILE *fi, long *nrt, int **nr_valores, long nr, char *);
char **_ReadLine(FILE *fi, int *nrt, int nr, char *);
*/

/* US no longer used
char ***_ReadFile(FILE *fi, long *nrt, int *nr_valores, char *);
char **_ReadLine(FILE *fi, int *nrt, char *);
char *ReadField(FILE *fi, char *sep);
*/

#endif


/* ======================================================================
   VECTOR
   ---------------------------------------------------------------------- */
/* ******************************************* */
/* Functions for vector and matrix             */
/* Memory Allocation   - Header File           */
/* ******************************************* */
int **imatrix(long nrl,long nrh, long ncl, long nch);
int *ivector(long nrl, long nrc);
void free_ivector(int *v, long nl, long nh);
void free_imatrix(int **m, long nrl, long nrh, long ncl, long nch);

double *dvector(long nrl, long nrh);
double **dmatrix(long nrl,long nrh, long ncl, long nch);
double ***dtensor(long nrm, long nrcol, long nrlin);

void free_dvector(double *v, long nl, long nh);
void free_dmatrix(double **m, long nrl, long nrh, long ncl, long nch);
void ShowMatrix(double **m, int n0, int n, int l0, int l);
void ShowCovMatrix(double **m, int n0, int n, int l0, int l);

/* TK */
void PrintMatrix (double **m, FILE *stream, int n0, int n, int l0, int l);
extern int debug;
extern char *debug_file;
#define DEBUGFP		(fopen (debug_file, "a"))
/* TK. */

/* ======================================================================
   FUNCTIONS
   ---------------------------------------------------------------------- */

#ifndef sqr
#define sqr(a)   ((a)*(a))
#endif


#define TypeOfVal(a_v)      a_v.type_of_val
#define CVal(a_v)           a_v.val.c
#define DVal(a_v)           a_v.val.d

#define NormalVal(a_v)      (a_v.type_of_val == normal)
#define UnknownVal(a_v)     (a_v.type_of_val == unknown)
#define DontCareVal(a_v)    (a_v.type_of_val == dontcare)

#define NameAttr(D,i)         D->attrs[i].name
#define TypeAttr(D,i)         D->attrs[i].type_attr
#define GlobalNum(D,i)        D->attrs[i].global_num

#define ValsAttr(D,i)         ((char **)D->attrs[i].vals)
#define SValsAttr(D,i)        ((List *)D->attrs[i].vals)
#define NValsAttr(D,i)        D->attrs[i].nr_vals

#define DomainName(D)               D->root_name
#define NrAttrs(D)                  D->nr_attrs

#define ContAttr(D,attr)        (TypeAttr(D,attr) == continuous)
#define NomAttr(D,attr)         (TypeAttr(D,attr) == nominal)
#define IntAttr(D,attr)         (TypeAttr(D,attr) == integer)
#define OrdAttr(D,attr)         (TypeAttr(D,attr) == ordered)
#define StructAttr(D,attr)      (TypeAttr(D,attr) == structured)

#define DValAttEx(instance, att) instance[att].val.d
#define CValAttEx(instance, att) instance[att].val.c
#define Id(instance)             instance[0].val.c

#define ForAllAttributes(D, attr)     for(attr = 1; attr <= D->nr_attrs; attr++)


/* INSTANCES */
#define Permute(ds)               SeedPermute(ds, 1, ds->nr_exs, time(NULL) / 2)
#define IntPermute(ds, low, high) SeedPermute(ds, low, high, time(NULL) / 2)
int SeedPermute(Dataset *ds, unsigned long Low, unsigned long High, int seed);

Dataset *ReadDataset(FILE *fi, DomainInfo *domain);

List *LReadInstances(FILE *fi, DomainInfo *domain, unsigned long *nrexs);
void Show_LInstances(DomainInfo *domain, List *instances, int low, int high);

AttrVal *ReadInstance(FILE *fi, DomainInfo *domain, unsigned long nrex);
void ShowInstances(Dataset *ds, unsigned long low, unsigned long high);
void ShowInstance(DomainInfo *, AttrVal *instance);
void Exchange(Instance *, unsigned long, unsigned long);

int QuickSort(Dataset *, int, unsigned long, unsigned long);
int JoinValues(Dataset *, int, int, unsigned long, unsigned long);
int SplitingPosition(Dataset *, int, unsigned long, unsigned long, double);

double CrossValidation(Dataset *, int, unsigned long, unsigned long, 
double(*func)(Dataset *, unsigned long), double *);
double StratifiedCrossValidation(Dataset *, unsigned long, unsigned long, int, 
int, double(*func)(Dataset *, unsigned long), double *);

#define WriteToFile(fo, dataset, low, high) _WriteToFile(fo, dataset, low, high, "\t", "\n")
int _WriteToFile(FILE *, Dataset *, unsigned long, unsigned long, char *, char *);

/* PROBLEM INFORMATION */
#define ReadDomain(F)        _ReadDomain(F, _DOMAIN_FILE_EXT)
DomainInfo *_ReadDomain(char *,char *);

#define NR_EXAMPLES(ds) (ds->nr_exs)
#define DOMAIN_INFO(ds)      (ds->domain)
#define INSTANCES(ds)   (ds->instances)
#define INSTANCE(ds, i)   (ds->instances[i])

unsigned int IdValLbl(DomainInfo *, unsigned int IdAttr,char *Lbl);
char *LblValId(DomainInfo *, unsigned int IdAttr,unsigned int IdVal);
unsigned int IdAttr(DomainInfo *, char *);
int IsA(unsigned int Spec, unsigned int Gen, List *values);

void SetUnknownChar(char );
void SetDontcareChar(char );

void ShowDomain(DomainInfo *);
void ShowTarget(DomainInfo *);

/* U T I L S */
int ***Distribuitions(Dataset *ds, int goal, unsigned long Low, unsigned long High);
void ShowDistributions(DomainInfo *domain, int ***dist, int goal, 
		       unsigned long nr_exs);
double ***Moments(Dataset *ds, int goal, unsigned long Low, unsigned long High);
void ShowMoments(DomainInfo *domain, double ***moments, int goal);

/* M A T R I C E S  */
double **jacobi(double **, int , double *, int *);
void sort_eigen(double *, double **, int *, int, int);
void balanc(double **, int); /* (TK): used to be static */
extern void balanc(double **, int);

int ludcmp(double **m, int n, int *i, double *d);
void lubksb(double **x, int n, int *i, double *y);
double **lu_inversa(double **m, int n);
double **lu_pmi(double **m1, double **m2, int n);
int lu_solve(double **x, int n, double *y);
int lu_msolve(double **x, int n, double **y);
double lu_determinant(double **m, int n);

int choldc(double **m, int n, double *p);
void cholsl(double **x, int n, double *d, double *y, double *r);
double **cholinv(double **x, int n);
double **cholpmi(double **x, double **y, int n);



/* ======================================================================
   Debug support for MS VC++ (added by W. Kwedlo)
   ---------------------------------------------------------------------- */
#if defined _DEBUG && defined WIN32
#define  _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#endif
