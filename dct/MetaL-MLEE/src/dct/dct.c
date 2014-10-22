/*
 * $Source /user1/grimmer/clem/src/achim/src/RCS/dct.c,v $
 * $Author: johann $
 * $Date: 2002/06/30 18:02:16 $
 * $Revision: 1.1.1.1 $
 */
/*
 * Revision 5.2 2001/03/09 15:10:10 koepf
 * changed calculation of Mean_skew so that only
 * those values are used that are non-NaN
 *
 * Revision 5.1 2001/02/23 17:22:15 koepf
 * added function ShowCovMatrix so that covariance
 * matrix is given out in standard form
 *
 * Revision 5.0 2000/11/17 19:20:20 koepf
 * changed dct to be able to cope with new data
 * format and fixed some small bug causing core
 * dumps when dataset name was read in
 * number of missing values is given out now 
 * every time
 * information theoretical measures are given
 * out only when they are calculated 
 *
 *  $Log: dct.c,v $
 *  Revision 1.1.1.1  2002/06/30 18:02:16  johann
 *  Initial CVS version
 *
 *  Revision 4.7  1999/06/14 09:57:52  lindner
 *  computes class entropy every time
 *
 * Revision 4.6  1998/10/26  15:33:14  spuhl
 * *** empty log message ***
 *
 * Revision 4.5  1998/09/18  15:57:24  spuhl
 * changed truncating to rounding for rules file
 *
 * Revision 4.4  1998/09/17  13:03:16  spuhl
 * changed methods to write domain and rule file
 * both files now can be used as source files for generator
 * kept order of attributes
 * set sum of activation in most cases to 1.0 (fix last attribute)
 *
 * Revision 4.3  1998/09/16  09:04:40  spuhl
 * fixed bug in writing rule file (if exists missing value)
 *
 * Revision 4.2  1998/07/17  12:33:38  kleiner
 * changed VERSION back to 2.7 instead of 3.0
 * we didn't really go that far
 * (didn't change program itself!!!)
 *
 * Revision 4.1  1998/07/17  11:52:03  kleiner
 * switched to revision 4.1
 * starting version 3.0
 *
 * Revision 1.7  1998/07/15  15:09:41  spuhl
 * write rules for missing values in rules file
 *
 * Revision 1.6  1998/07/15  08:13:04  spuhl
 * avoided recursion
 * Names2Domain unneeded
 *
 * Revision 1.5  1998/07/09  08:55:12  spuhl
 * fixed error in typecast
 *
 * Revision 1.4  1998/07/06  14:56:41  kleiner
 * added Missing values to save_datadict_to_file
 *
 * Revision 1.3  1998/07/02  16:59:24  spuhl
 * fixed german output
 *
 * Revision 1.2  1998/07/02  08:23:58  spuhl
 * *** empty log message ***
 *
 * Revision 1.1  1998/06/29  11:11:13  spuhl
 * Initial revision
 *
 * Revision 3.7  1998/06/29  06:27:08  spuhl
 * changed output of Measures.data
 * added results.h
 *
 * Revision 3.6  1998/06/25  07:36:30  spuhl
 * *** empty log message ***
 *
 * Revision 3.5  1998/06/25  07:24:45  kleiner
 * changed from mygetopt to standard getopt
 * using getopt.h and getopt.c
 *
 * Revision 3.4  1998/06/25  07:04:25  spuhl
 * added option -M
 *
 * Revision 3.3  1998/06/24  09:50:53  spuhl
 * added output of covariance matrix (save_data_dict_to_file)
 *
 * Revision 3.2  1998/06/24  08:43:42  spuhl
 * adjusted error in calculating confidence matrix (check estatistica.c)
 *
 * Revision 3.1  1998/06/23  12:40:53  kleiner
 * changed to revision 3 for all files
 *
 * Revision 2.27  1998/06/23  11:08:29  kleiner
 * added saving of Rule file for (-m / -D)
 *
 * Revision 2.26  1998/06/22  12:30:40  kleiner
 * added dcdflib.c cdflib.h ipmpar.c
 *  => calcualtion of chi^2-values for m- and v-statistics
 *
 * Revision 2.25  1998/06/16  16:30:21  kleiner
 * changed VERSION to 2.5
 * Reason:
 *   2.4 and prior had an error handling continuous missing values
 *   (they were used to calculate the moments being initialized to 0)
 *
 * Revision 2.24  1998/06/12  12:25:09  thor
 * changed ShowDistribution
 *
 *  Revision 2.23  1998/06/09 07:50:23  kleiner
 *  added warning for default activations
 *
 * Revision 2.22  1998/06/09  07:22:10  kleiner
 * added output of default activations for symbolic attributes
 * (save_data_dict_to_file)
 *
 * Revision 2.21  1998/06/06  17:06:35  thor
 * added extended missing value estimation and output
 *
 *  Revision 2.20  1998/06/05 14:36:46  kleiner
 *  added save_datadict_to_file
 *
 * Revision 2.19  1998/06/04  16:13:44  kleiner
 * added 'isint' => integer variables are set continuous
 * 'isint' is needed to distinguish between integer and continuous
 * to be able to write an extensive Data Dictionary to a file
 *
 * Revision 2.18  1998/06/04  10:46:52  kleiner
 * added confidence intervals for mean to MyShowMoments
 * revived 'stat.c'
 *
 * Revision 2.17  1998/06/02  16:32:32  kleiner
 * *** empty log message ***
 *
 * Revision 2.16  1998/06/02  16:28:02  kleiner
 * added Option [-B value] (set BHEP_BETA)
 * added output of critical value vor BHEP_ALPHA = 0.05 and 0.1
 * inverse
 * ...
 *
 * Revision 2.15  1998/05/27  14:05:10  kleiner
 * aligned some output
 * added output of correlations
 *
 * Revision 2.14  1998/05/25  16:04:24  kleiner
 * *** empty log message ***
 *
 * Revision 2.13  1998/05/20  10:45:45  kleiner
 * MAX
 * Read
 * Domain
 *
 * Revision 2.12  1998/05/18  07:36:33  kleiner
 * *** empty log message ***
 *
 * Revision 2.11  1998/05/18  07:11:49  kleiner
 * *** empty log message ***
 *
 * Revision 2.10  1998/05/18  07:03:35  kleiner
 * removed ^M from all files!!!!
 *
 * Revision 2.9  1998/05/17  18:22:00  Administrator
 * changed output for unknown option
 *
 *  Revision 2.8  1998/05/17 17:59:41  Administrator
 *  *** empty log message ***
 *
 *  Revision 2.7  1998/05/17 17:55:36  Administrator
 *  adjusted USAGE (added Parameter -p)
 *
 *  Revision 2.6  1998/05/17 17:46:16  Administrator
 *  *** empty log message ***
 *
 *  Revision 2.5  1998/05/17 17:41:27  Administrator
 *  removed test output to Measures.data!
 *
 *  Revision 2.4  1998/05/17 17:34:41  Administrator
 *  adjusted writing to 'Measures.data' wasn't correct yet
 *  added parameter -p for PROGRESS INFORMATION
 *  added output of NR_UNKOWN (missing values) (to statistics.c)
 *
 *  Revision 2.3  1998/05/16 15:03:28  Administrator
 *  changed nothing!
 *
 *  Revision 2.2  1998/05/15 14:22:43  kleiner
 *  coordinated get_time calls
 *  added fflush's
 *  ...
 *
 * Revision 2.1  1998/05/15  07:10:33  kleiner
 * original from latest version (analist v2.2) now heading
 * for analist 3.0 ;-)
 * this message counts for all files in the rcs-archiv!
 * (from rev 2.1 some things will change)
 *
 * Revision 1.6  1998/05/15  07:04:35  kleiner
 * *** empty log message ***
 *
 * Revision 1.5  1998/05/14  16:38:50  kleiner
 * removed rlimit (Stack - checking)
 * changed recursion to iterativ reading of data file
 *  => from now on also more then 60000 examples work
 *
 * Revision 1.4  1998/05/13  13:44:18  kleiner
 * version 2.2
 * + qsort
 * + ABS
 * + INFILE
 * + optimized bhep_factor
 * + ...
 *
 * Revision 1.3  1998/05/12  14:56:52  kleiner
 * added some output
 *
 * Revision 1.2  1998/05/07  16:18:14  kleiner
 * changed options and globals
 *
 */

/*********************************************************************/
/*        "Analist"  FOR PROPOSITIONAL LEARNING                      */
/*         Versao 2.0 (05/02/1997)  FILE: analist.c                  */
/*               Developed by: Joao Gama                             */
/*                LIACC - Uni.do Porto                               */
/*                jgama@ncc.up.pt                                    */
/*        Further development by Ulrike Zintz and Guido Lindner      */
/*        Version 3.0 (31/03/1998)                                   */
/*             Daimler Benz Research AG                              */
/*             FT 3 KL                                               */
/*             lindner@str.Daimler-Benz.com                          */
/*********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <time.h>
#include <limits.h>
#include <string.h>
#include "framework.h"
#include "measures.h"
#include "estatistica.h"
#include "set_var.h"
#include "entropia.h"  /* (TK) */
#include "getopt.h"    /* (TK) */
#include "results.h"   /* US */

#define USAGE \
fprintf (stderr, "Use: %s -f dataset [-m name] [-M] [-v verbose] [-b] [-p dsname] [-c] [-B value]\n\n", argv[0]); \
fprintf (stderr, "\tf:\tspecifies the dataset to use\n"); \
fprintf (stderr, "\tm:\tsets root-name for generator-files to name\n"); \
fprintf (stderr, "\ts:\tsets root-name for result-file to name\n"); \
fprintf (stderr, "\tv:\tsets the verbosity level [0..8]\n"); \
fprintf (stderr, "\tb:\tskip BHEP-Test\n"); \
fprintf (stderr, "\tp:\tshow progress information\n"); \
fprintf (stderr, "\tc:\tname of DataSet for short result-file\n"); \
fprintf (stderr, "\tB:\tset BHEP_BETA for BHEP-Test\n"); \
fprintf (stderr, "\tD:\tsave Data Dictionary to file\n\n"); \
fprintf (stderr, "\texpecting data-file:\t'dataset.data'\n"); \
fprintf (stderr, "\texpecting domain-file:\t'dataset.names'\n\n");


/* (TK):
 *  taken from 'set_var.h'
 *  (used to be static)
 */
int nr_of_examples[6] =          /* categorization for the number of examples */
{200,500,2000,5000,10000,20000};

char nr_of_categories[6] = 
{'A','B','C','D','E','F'};

double alpha_phi[10][2] = {       /* values for alpha and the corresponding */  
  {0.01,      2.97},              /* phi(alpha) (extracted from a table)    */   
  {0.02,      2.31}, 
  {0.03,      1.88},
  {0.04,      1.75},
  {0.05,      1.65},
  {0.06,      1.56},
  {0.07,      1.48},
  {0.08,      1.41},
  {0.09,      1.34},
  {0.1,	      1.28}
};
/* (TK): end */


#define FALSE 0
#define TRUE 1

#define VERSION		3.2

int debug = FALSE;
char *debug_file = NULL;


short VERBOSITY = 0;
#define VERBOSE(v) if(VERBOSITY >= v)

#define MAXFILENAMELEN		100;

/*char *optarg = NULL;*/

int RESFILE = FALSE;
int NR_UNKNOWN = 0; /* TK: changed UNKNOWN to NR_UNKNOWN */
int NR_LINES_UNKNOWN = 0; /* TK: count nr of lines with missing values */
int NR_OUTLIERS;  /* TK: calculated by quantile.c */


/* TK: */
int	BHEPTEST = TRUE;
int entr_calculated = TRUE;
int mcorr_calculated = TRUE;
int quant_calculated = TRUE;
int	PROGINFO = FALSE; /* Show Progress Informations (on stderr) */
double  bhep_beta = BHEP_BETA;

int	SAVE_DATADICT = FALSE;
int     SAVE_RESULTS = FALSE;

void    save_datadict_to_file (char* name, FILE *dom_fp, FILE *rule_fp,
			       FILE *covar_fp, DomainInfo *sym, 
			       DomainInfo *num, double ***mom, int ***dist, 
			       double ***covar, unsigned long nr_exs,
			       DomainInfo *global);

double rint(double num);

DomainInfo *MyReadDomain(char *filename);
void Names2Domain(DomainInfo *names);

/* (TK): Forward Declaration */
/* int mygetopt(int Argc, char **Argv,char *Str); */   /* no longer used */
extern void ShowVals();  /* defined in helpfunc.c */
extern void Show_Instances(Dataset *ds, int low, int high); /* instances.c */
/* (TK): end */

/******************************/
/*    MAIN FUNCTION           */
/******************************/
int 
main (int argc, char *argv[]) {
  char opt, *FileName = NULL;
  char *GenFileName = NULL;
  char *ResFileName = NULL;
  char *DataSetName = NULL;
  FILE *fdata = NULL;
  DomainInfo *names = NULL;
  Dataset *ds = NULL;
  long int hora;
  int ***dist, i, l;
  double ***mom=NULL, ***covar=NULL, **cov1=NULL;
  double **matrix = NULL, **mat = NULL, **new_cov = NULL;
  AttrVal ***inst = NULL;
  Dataset sym, num;
  DomainInfo domain_num, domain_sym;
  DomainInfo *res = NULL;
  int nr_num, nr_sym;
  double *eig_val = NULL;
  double crit_val;
  int zero_missing_values = 0;

  FILE *debug_fp = NULL;
  FILE *dict_fp = NULL;
  FILE *rule_fp = NULL;
  FILE *covar_fp = NULL;
  FILE *results_fp = NULL;
  FILE *shortresults_fp = NULL;


#if defined _DEBUG && defined WIN32
 _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif


  
  
  hora = time(0);
  fprintf(stdout, "DCT (Version %1.1f) \n%s\n", VERSION, ctime(&hora));
  fflush (stdout);
  
  if (PROGINFO) {
    fprintf(stderr, "DCT (Version %1.1f) \n%s\n", VERSION, ctime(&hora));
    fflush (stderr);
  }
  
  if (argc < 2) {
    USAGE;
  }
  else { 
    while ((opt = getopt(argc, argv, "hbpzDf:v:m:s:B:c:o:")) != -1) {

    /*while ((opt = mygetopt (argn, argl, "hbpzDMf:v:m:B:")) != EOF ) {*/

      switch (opt)      {
      case 'h':
	USAGE;
	return 0;
	break;
      case 'f':
	FileName = optarg; 
        break;

      case 'v':
	if (sscanf (optarg, "%hd", &VERBOSITY) != 1) {
	  fprintf (stderr, "ERROR: parameter -v [%s]\n", optarg) ;
	  exit(1) ;
	}
	if ((VERBOSITY < 0) || (VERBOSITY > 8)) {
	  fprintf (stderr, "ERROR: choose verbosity from [0..8]\n");
	  fflush (stderr);
	  USAGE;
	  exit (2);
	}
/*       case 'v': */
/*         VERBOSITY = atoi(optarg); */

        break;

      case 'm':
        GenFileName = optarg;
	SAVE_DATADICT = TRUE;
        break;

      case 's':
	ResFileName = optarg;
	SAVE_RESULTS = TRUE;
        break;	
      case 'b':
	BHEPTEST = FALSE;
	break;
      case 'p':
	PROGINFO = TRUE;
	break;
	  case 'o':
		  for(i=0;optarg[i]!='\0';i++)
			  switch(optarg[i]) {
				case 'e' : entr_calculated=FALSE;
						   break;
				case 'b' : BHEPTEST=FALSE;
						   break;
				case 'q' : quant_calculated=FALSE;
						   break;
				case 'c' : mcorr_calculated=FALSE;
						   break;
				case 'm' : m_calculated=FALSE;
						   break;
				case 'v' : v_calculated=FALSE;
						   break;
				}
		  break;
      case 'c':
	RESFILE = TRUE;
	DataSetName = optarg;
	break;
      case 'B':
	if (sscanf (optarg, "%lf", &bhep_beta) != 1) {
	  fprintf (stderr, "ERROR: parameter -B [%s]\n", optarg) ;
	  exit(5) ;
	}
/* 	bhep_beta = atof (optarg); */
	if (bhep_beta <= 0) {
	  fprintf (stderr, "\nBHEP_BETA is supposed to be > 0\n\n");
	  USAGE;
	  exit (1);
	}
	break;
      case 'D':
	SAVE_DATADICT = TRUE;
	break;
      case 'z':
	debug = TRUE;
	break;
      default:
        fprintf(stderr, "\nUnknown Option '-%c'\n\n", opt);
	USAGE;
        exit(1);
      }
    }
    
    if (ResFileName == NULL) { 
      ResFileName = FileName; 
    } 
    

    if (debug) {
      debug_file = (char *) malloc ((strlen (FileName) + 7) * sizeof (char));
      strcpy (debug_file, new_strcat (FileName, ".debug"));
      if ((debug_fp = fopen (debug_file, "w")) == NULL) {
	fprintf (stderr, "\nERROR could not open debug file\n");
      debug = FALSE;
      }
      else {
	fclose (debug_fp);
      }
    }

    InitResults();

    /* Show the used constants etc. */
    ShowVals(); 
    fprintf (stdout, "\t\t\t\t#***  RESULTS ***\t\t\n\n"); 
    fflush (stdout);
    if ((names = MyReadDomain(FileName)) != NULL) {
      if ((fdata = fopen(new_strcat(FileName, ".data"), "r")) != NULL) {
	ds = ReadDataset(fdata, names);   
     
        res = (DomainInfo *) calloc (2, sizeof(DomainInfo));  
        res = make_domain(names, NrAttrs(names));
	domain_num = res[0];
	domain_sym = res[1];
     
        num.domain = &domain_num;
        sym.domain = &domain_sym; 
        num.nr_exs = ds -> nr_exs;
        sym.nr_exs = ds -> nr_exs;
       
        nr_num = (num.domain) -> nr_attrs;
        nr_sym = (sym.domain) -> nr_attrs;             
        
        inst = (AttrVal ***) calloc (2, sizeof(AttrVal **));
        inst = get_instances(ds, nr_num, nr_sym);
        num.instances = inst[0];
        sym.instances = inst[1];
	
        get_time ("read dataset"); /* TK */
        print_line();
        fflush (stdout);

        dist = Distribuitions(&sym, nr_sym, 1, sym.nr_exs);
	
        VERBOSE(4) {
	  fprintf (stdout, "\n#*** Show domain of target attribute ***\n\n");
	  ShowTarget(sym.domain);
	  print_line();
	  fprintf (stdout, "\n#*** Show domain of numerical attributes ***\n\n");
	  ShowDomain(num.domain);
	  print_line();
	  fprintf (stdout, "\n#*** Show domain of symbolic attributes ***\n\n");
	  ShowDomain(sym.domain);
	  print_line();
        }
       
        VERBOSE(5) {
	  fprintf (stdout, "\n#*** Show numeric instances and target attribute ***\n\n");
	  for(i = 1; i <= nr_num; i++)
	    fprintf (stdout, "%7s\t",num.domain -> attrs[i].name);
	  fprintf (stdout, "\n");  
	  Show_Instances(&num, 1, num.nr_exs);
	  print_line();
	  fprintf (stdout, "\n#*** Show symbolic instances and target attribute***\n\n");
	  for(i = 1; i <= nr_sym; i++)
	    fprintf (stdout, "%7s\t",sym.domain -> attrs[i].name);
	  fprintf (stdout, "\n");  
	  Show_Instances(&sym, 1, sym.nr_exs);
	  print_line(); 
        }     

        ShowStat(&sym,&num,dist);  

	results[NR_TUPEL] = (double) ds->nr_exs;

	/* TK: */
	if (NR_UNKNOWN > 0) {
	  print_line ();
	  fprintf (stdout, "\nMissingValues_Total: %5d\nMissingValues_Relative: %1.2f\n", 
		   NR_UNKNOWN, 
		   (double) NR_UNKNOWN/(ds->nr_exs*ds->domain->nr_attrs));
	  fprintf (stdout, "Lines_with_MissingValues_Total: %5d\nLines_with_MissingValues_Relative: %3.2f\n\n", 
		   NR_LINES_UNKNOWN, (double) NR_LINES_UNKNOWN/ds->nr_exs
		   );

	  results[MISS_VAL] = (double) NR_UNKNOWN / (double) (ds->nr_exs*ds->domain->nr_attrs);
	  results[MISS_VAL_LINES] = (double) NR_LINES_UNKNOWN / (double) ds->nr_exs;
	  
	  for (i=1; i<=ds->domain->nr_attrs; i++) {
	    if (ds->domain->attrs[i].nr_unknown > 0) {
//	      fprintf (stdout, "%s: ", ds->domain->attrs[i].name);
//	      for (l=strlen(ds->domain->attrs[i].name); l<=10; l++)
//		fprintf (stdout, " ");
	      fprintf (stdout, "Attr_MissingValues_%d: %d\n", i, ds->domain->attrs[i].nr_unknown);

	    }
	  }

	  fflush (stdout);
	}

	/* change koepf */

	else {
	
	  print_line ();
	  fprintf (stdout, "\nMissingValues_Total: %5d\nMissingValues_Relative: %5d\n", 
		   zero_missing_values, zero_missing_values);
	  fprintf (stdout, "Lines_with_MissingValues_Total: %5d\nLines_with_MissingValues_Relative: %3d\n\n", 
		   zero_missing_values, zero_missing_values);

	  results[MISS_VAL] = (double) NR_UNKNOWN / (double) (ds->nr_exs*ds->domain->nr_attrs);
	  results[MISS_VAL_LINES] = (double) NR_LINES_UNKNOWN / (double) ds->nr_exs;
	  
	  for (i=1; i<=(ds->domain->nr_attrs)-1; i++) {
	      fprintf (stdout, "Attr_MissingValues_%d: %d\n", i, zero_missing_values);
	  }

	  fflush (stdout);
	}

	/* end koepf */

	/* TK end. */

	get_time("Statistic");
        print_line();
	
        if (nr_sym >= 1) { 
	  ShowDistributions(sym.domain, dist, nr_sym, sym.nr_exs);
	  print_line();
        }
        
        if (nr_num > 1) {        
	  mom = MyMoments(&num, nr_num, 1, num.nr_exs);
	  MyShowMoments(num.domain, mom, nr_num);
	  get_time("attribute statistics");
	  print_line();
		   
	  if (PROGINFO) {
	    fprintf (stderr, "*** Starting to compute the quantiles\n");
	  }
	  if (quant_calculated)
	  {
		show_lage_streu(&num); 

		results[OUTLIERS] = NR_OUTLIERS;
	  
		get_time ("Quantile");
		print_line();
	  }
        
        }
        
                
	if (NValsAttr(names, NrAttrs(names))) {  /* TK ??????? */
	  if (nr_num > 1) { 
	    VERBOSE(2)  
	      fprintf (stdout, "\n#*** Covariance-Matrixes ***\n");  
      
	    covar = covariance(&num, NrAttrs(num.domain), mom, dist[NrAttrs(sym.domain)]); 
	    VERBOSE(2)
	      print_line();							  

	    mat = read_numeric_old(&num);   
	    

	    new_cov = move_index(covar[0], NrAttrs(num.domain)-1, NrAttrs(num.domain)-1);

	    fprintf (stdout, "\n#*** Preconditions for V-Statistic ***\n\n");
	    fprintf (stdout, "#1. Coefficient of Multiple Correlation\n");
	    fprintf (stdout, "#2. BHEP-Test\n");
	    fprintf (stdout, "#3. M-Statistic\n");
	    fprintf (stdout, "#4. Sd-Ratio\n");
	    fflush (stdout);

	    if (mcorr_calculated)
		{
			show_multi_cor(new_cov, NrAttrs(num.domain)-1,&num);
			get_time("Multiple Correl");
		}
	    
	    
		if (BHEPTEST) { /* TK */
	      bhep_calculated = TRUE;
	      fprintf (stderr, "*** Starting BHEP-Test\n");
	      matrix = bhep_matrix(covar[0], NrAttrs(num.domain)-1);
	      show_bhep_test(mat, num.nr_exs, NrAttrs(num.domain)-1, matrix, bhep_beta);
	      get_time ("BHEP-Test");
//		fprintf (stdout, "\n\tBHEP_ALPHA = %1.2f  BHEP_BETA = %1.2f:\n", 
//		     BHEP_ALPHA, bhep_beta);
		
	      fprintf (stdout, "BHEP_BETA: %1.2f:\n",bhep_beta);

	      crit_val = calc_crit_value(bhep_beta, NrAttrs(num.domain)-1, phi_func(BHEP_ALPHA));
	      results[BHEP_01] = crit_val;
//	      fprintf(stdout, "\tCritical value = %1.3f\n", crit_val);

	      fprintf(stdout, "BHEPCriticalValue(Alpha: %1.4f): %1.3f\n", BHEP_ALPHA,crit_val);
	      
//	      fprintf (stdout, "\n\tBHEP_ALPHA = %1.2f  BHEP_BETA = %1.2f:\n", 
//		     0.05, bhep_beta);
	      crit_val = calc_crit_value(bhep_beta, NrAttrs(num.domain)-1, phi_func(0.05));
	      results[BHEP_005] = crit_val;
	      	      
	      fprintf(stdout, "BHEPCriticalValue(Alpha: %1.4f): %1.3f\n", 0.05,crit_val);
//		fprintf(stdout, "\tCritical value = %1.3f\n", crit_val);

//	      fprintf (stdout, "\n\tBHEP_ALPHA = %1.2f  BHEP_BETA = %1.2f:\n", 
//		     0.01, bhep_beta);
	      crit_val = calc_crit_value(bhep_beta, NrAttrs(num.domain)-1, phi_func(0.01));
	      results[BHEP_001] = crit_val;
//	      fprintf(stdout, "\tCritical value = %1.3f\n", crit_val);
		
		fprintf(stdout, "BHEPCriticalValue(Alpha: %1.4f): %1.3f\n", 0.01,crit_val);
	      fflush (stdout);
	      get_time ("Critical value"); /* TK */
	    }
	    else {
	      fprintf (stderr, "\n*** Skipping BHEP-Test\n");
	      fprintf (stdout, "\n#*** Skipped BHEP-Test\n");
	      fflush (stderr);
	      fflush (stdout);
	    } /* TK */

	    if (m_calculated)
		{
			m_calculated=FALSE;
			ShowMStat(&num, covar, dist[NrAttrs(sym.domain)]);   
			get_time ("SD-ratio"); /* TK */
		}
	    if (v_calculated)
		{
			v_calculated=FALSE;
			eig_val = pca(num.domain, covar, \
				NValsAttr(num.domain, NrAttrs(num.domain)),dist[NrAttrs(sym.domain)]); 
			show_v_stat(eig_val, &num); 
			get_time("V-Statistic"); 
			print_line();
		}
	   
	    VERBOSE(3) {
	      fprintf (stdout, "\n#*** Correlation-Matrixes ***\n");
	      correlacoes(&num, NrAttrs(num.domain), mom, covar);
	      print_line();
	      /* TK */
	      fprintf (stdout, "#*** Global Covariance-Matrix ***\n"); 
	      cov1 = covariance1(&num, mom);
	      fprintf (stdout, "#*** Global Correlation-Matrix ***\n"); 
	      correlacao(&num, mom, cov1);
	      print_line ();
	      pca1(num.domain, cov1);   
	      /* TK */
	    }
          
          }      
       	}
	else { 
	  cov1 = covariance1(&num, mom);
	  correlacao(&num, mom, cov1);
	  pca1(num.domain, cov1);   
	}
      

	  print_line();
	  fprintf (stdout, "\n#*** Information-theoretic Measures ***\n"); 

	  /* change koepf beginning */

	  if (REGRESSION){
	    ;
	  }
	  else

	    /* change koepf end */

	if (nr_sym > 1 && entr_calculated) {
	  show_gini(dist,&sym);
	  get_time("Gini-Index");
	  show_relevanz(dist,&sym);
	  get_time("Measure of Relevance");  
	  show_g_func(dist,&sym);  
	  get_time("g-Function");
        }  
	/* change entropie measures are compute ever time GL */
	if (entr_calculated)
		entropia(&sym, dist, NrAttrs(sym.domain));
	get_time("entropia");
	  print_line();
		

	/* Now that we are done we could save the data dictionary */
	/* Should we?  (TK)*/
	if (SAVE_DATADICT) {

	  fprintf (stderr, "\n*** Saving data dictionary to '%s'\n",
		   new_strcat(GenFileName, ".dict"));
	  fprintf (stderr, "*** Saving rules to '%s'\n",
		   new_strcat(GenFileName, ".rules"));
	  fprintf (stderr, "*** Saving covariance structure to '%s'\n\n",
		   new_strcat(GenFileName, ".covar"));
	  fflush (stderr);

	  if ((dict_fp = fopen (new_strcat(GenFileName, ".domain"), "w")) != NULL) {

	    if ((rule_fp = fopen (new_strcat(GenFileName, ".rules"), "w")) 
		!= NULL) {

	      if ((covar_fp = fopen (new_strcat(GenFileName, ".covar"), "w")) 
		  != NULL) {

		save_datadict_to_file (GenFileName, dict_fp, rule_fp, covar_fp,
				       sym.domain, num.domain, mom, dist, 
				       covar, sym.nr_exs, names);
		fclose(covar_fp);
	      }
	      else {
		fprintf (stderr, "Couldn't open '%s.covar'\n", GenFileName);
		fflush (stderr);
		/*return (101);*/
	      }
	      fclose(rule_fp);
	    }
	    else {
	      fprintf (stderr, "Couldn't open '%s.rules'\n", GenFileName);
	      fflush (stderr);
	      /*return (102);*/
	    }
	    fclose(dict_fp);
	  }
	  else {
	    fprintf (stderr, "Couldn't open '%s.dict'\n", GenFileName);
	    fflush (stderr);
	    /*return (103);*/
	  }
	}
        if (SAVE_RESULTS) {
	  fprintf (stderr, "*** Saving reults to '%s'\n\n",
	           new_strcat(ResFileName, ".res"));
	  fflush (stderr);

          if ((results_fp = fopen (new_strcat(ResFileName, ".res"), "w")) 
	       != NULL) {
	    if(WriteResultFile(results_fp, ResFileName, FileName, TRUE)) {
              fprintf (stderr, "*** Error saving reults to '%s'\n\n",
	           new_strcat(ResFileName, ".res"));
	      fflush (stderr);
            }
	    fclose(results_fp);
	  }
	  else {
	    fprintf (stderr, "Couldn't open '%s.res'\n", ResFileName);
	    fflush (stderr);
	  }
	}
	if (RESFILE) {
	  fprintf (stderr, "*** Saving short reults to '%s'\n\n",
	           new_strcat(DataSetName, ".sres"));
	  fflush (stderr);

          if ((shortresults_fp = fopen (new_strcat(DataSetName, ".sres"), "w")) 
	       != NULL) {
	    if(WriteResultFile(shortresults_fp, DataSetName, FileName, FALSE)) {
              fprintf (stderr, "*** Error saving short reults to '%s'\n\n",
	           new_strcat(DataSetName, ".sres"));
	      fflush (stderr);
            }
	    fclose(shortresults_fp);
	  }
	  else {
	    fprintf (stderr, "Couldn't open '%s.sres'\n", DataSetName);
	    fflush (stderr);
	  }
	}
      }
      else {
	fprintf (stderr, "Data File %s.data not found !\n", FileName);
	fflush (stderr);
	return 101;
      }
    }
    else {
      fprintf (stderr, "Domain File %s.domain or %s.names not found !\n", 
	       FileName, FileName);
      fflush (stderr);
      return 100;
    }
  }
  fflush (stderr);
  fflush (stdout);

  hora = time(0);
  fprintf(stdout, "DCT (Version %1.1f) %s\n", VERSION, ctime(&hora));
  fflush (stdout);

  return 0; /* (TK) */
}

DomainInfo *
MyReadDomain (char *FileName) {
  DomainInfo *names = NULL;

  /*  if ((names = _ReadDomain(FileName, ".domain")) == NULL) */
  if ((names = _ReadDomain(FileName, ".names")) != NULL) {
    /*  Names2Domain(names);  US no longer usede */
  }
  

  return names;
}


/* TK: */
void    
save_datadict_to_file (char* name, FILE *dom_fp, FILE *rule_fp,
		       FILE *covar_fp, DomainInfo *sym, 
		       DomainInfo *num, double ***mom, int ***dist, 
		       double ***covar, unsigned long nr_exs,
		       DomainInfo *global) {
  
  int	 i, j, l;
  int cur_sym = 0;
  int cur_num = 0;
  char   **values = NULL;
  long   hora = time(0);
  int    nr_attrs = (sym->nr_attrs-1)+(num->nr_attrs-1)+1;
  double sum = 0.0;
  char   **sym_vals = NULL;
  double sum2 = 0.0;
  int sum3 = 0;
  int diff;
  char *attr_name;
  int attr_value;
  double tmp_value;
  
  int first_value;
  char unknown_str [] = "?";
  
  if (dom_fp != NULL) {

    fprintf (dom_fp, "# DCT (Version %1.1f)  %s\n", 
	     VERSION, ctime(&hora));
    fprintf (dom_fp, "# -------------------------------------------------------------- #\n");
    fprintf (dom_fp, "# Data Dictionary for '%s'\n#\n", name);
    fprintf (dom_fp, "#   nr_exs:   %8ld\t\t", nr_exs);
    if (NR_UNKNOWN > 0) {
      fprintf (dom_fp, "(missing: %6d/%ld)", NR_UNKNOWN, nr_exs*nr_attrs);
    }
    fprintf (dom_fp, "\n#   nr_attrs: %8d\n", nr_attrs);
    fprintf (dom_fp, "# -------------------------------------------------------------- #\n");
    fprintf (dom_fp, "\n");
    fprintf (dom_fp, "# class attribute:\n");
    fprintf (dom_fp, "# %s: ", sym->attrs[sym->nr_attrs].name);
    for (l=strlen(sym->attrs[sym->nr_attrs].name); l<=8; l++)
      fprintf (dom_fp, " ");
    
    values = (char **) sym->attrs[sym->nr_attrs].vals;
    sum = 0.0;
    

    /* Missing Values (Class attribute) */
    if ((sym->attrs[sym->nr_attrs].nr_unknown) > 0) {
      fprintf (dom_fp, "?;%5.3f, ", 
	       (double )sym->attrs[sym->nr_attrs].nr_unknown/nr_exs);
      sum += (double )sym->attrs[sym->nr_attrs].nr_unknown/nr_exs;
    }

    for (i=1; i<sym->attrs[sym->nr_attrs].nr_vals; i++) {
      fprintf (dom_fp, "%s;%5.3f, ", values[i], 
	       (double) dist[sym->nr_attrs][0][i]/nr_exs);
      sum += (double) dist[sym->nr_attrs][0][i]/nr_exs;
    }
    fprintf (dom_fp, "%s;%5.3f\n", values[i], 
	     (double) dist[sym->nr_attrs][0][i]/nr_exs);
    sum += (double) dist[sym->nr_attrs][0][i]/nr_exs;
    if ((sum < 1.0 - EPSILON) || (sum > 1.0 + EPSILON)) {
      fprintf (dom_fp, "# warning: default activations don't sum up to 1.0 (sum: %f)\n", sum);
    }
    fflush (dom_fp);
    
    fprintf (dom_fp, "%s: ", sym->attrs[sym->nr_attrs].name);
    for (l=strlen(sym->attrs[sym->nr_attrs].name); l<=10; l++)
      fprintf (dom_fp, " ");
    
    values = (char **) sym->attrs[sym->nr_attrs].vals;
    
    for (i=1; i<sym->attrs[sym->nr_attrs].nr_vals; i++) {
      fprintf (dom_fp, "%s, ", values[i]);
    }
    fprintf (dom_fp, "%s\n", values[i]);
    fflush (dom_fp);
    
    for (i = 1; i < global->nr_attrs; i++) {
      switch (global->attrs[i].type_attr) {
      case nominal : 
	{
	  cur_sym++;
	  if(strcmp(global->attrs[i].name, sym->attrs[cur_sym].name)) {
	    fprintf (dom_fp, "\n#ERROR: attributes do not match: %s, %s\n", 
		     global->attrs[i].name, sym->attrs[cur_sym].name);
	  }
	  else {
	    
	    /* Show symbolic attributes with default activations */
	    fprintf (dom_fp, "#%s: ", sym->attrs[cur_sym].name);
	    
	    for (l=strlen(sym->attrs[cur_sym].name); l<=9; l++)
	      fprintf (dom_fp, " ");
	    
	    sum = 0.0;
	    /* Missing Values */
	    if ((sym->attrs[cur_sym].nr_unknown) > 0) {
	      fprintf (dom_fp, "?;%5.3f, ", 
		       (double )sym->attrs[cur_sym].nr_unknown/nr_exs);
	      sum += (double )sym->attrs[cur_sym].nr_unknown/nr_exs;
	    }
	    
	    values = (char **) sym->attrs[cur_sym].vals;
	    for (j=1; j<sym->attrs[cur_sym].nr_vals; j++) {
	      fprintf (dom_fp, "%s;%5.3f, ", values[j], (double) dist[cur_sym][0][j]/nr_exs);
	      sum += (double) dist[cur_sym][0][j]/nr_exs;
	    }
	    fprintf (dom_fp, "%s;%5.3f\n", values[j], (double) dist[cur_sym][0][j]/nr_exs);
	    sum += (double) dist[cur_sym][0][j]/nr_exs;
	    if ((sum < 1.0 - EPSILON) || (sum > 1.0 + EPSILON)) {
	      fprintf (dom_fp, "# warning: default activations don't sum up to 1.0 (sum: %f)\n", sum);
	    }
	    
	    /* Show symbolic attributes without default activations */
	    fprintf (dom_fp, "%s: ", sym->attrs[cur_sym].name);
	    for (l=strlen(sym->attrs[cur_sym].name); l<=10; l++)
	      fprintf (dom_fp, " ");
	  
	    values = (char **) sym->attrs[cur_sym].vals;
	    for (j=1; j<sym->attrs[cur_sym].nr_vals; j++) {
	      fprintf (dom_fp, "%s, ", values[j]);
	    }
	    fprintf (dom_fp, "%s\n", values[j]);
	  }
	  break;
	}
      case continuous : 
	{
	  cur_num++;
	  if(strcmp(global->attrs[i].name, num->attrs[cur_num].name)) {
	    fprintf (dom_fp, "\n#ERROR: attributes do not match: %s, %s\n", 
		     global->attrs[i].name, num->attrs[cur_num].name);
	  }
	  else {
	    if (num->attrs[cur_num].isint) {
	      fprintf (dom_fp, "%s: ", num->attrs[cur_num].name);
	      for (l=strlen(num->attrs[cur_num].name); l<=10; l++)
		fprintf (dom_fp, " ");
	      fprintf (dom_fp, "integer     [%.0f,%.0f]  ", 
		       rint (mom[cur_num][0][1]), rint (mom[cur_num][0][2]));
	      /* Missing Values */
	      if ((num->attrs[cur_num].nr_unknown) > 0) {
		fprintf (dom_fp, "{?;%5.3f, [U,%.0f,%.0f]}\n", 
			 (double ) num->attrs[cur_num].nr_unknown/nr_exs,
			 rint (mom[cur_num][0][1]), rint (mom[cur_num][0][2]));
	      }
	      else {
		fprintf (dom_fp, "\n");
	      }
	    }
	    else {
	      fprintf (dom_fp, "%s: ", num->attrs[cur_num].name);
	      for (l=strlen(num->attrs[cur_num].name); l<=10; l++)
		fprintf (dom_fp, " ");
	      fprintf (dom_fp, "continuous  [%.4f,%.4f]  ", 
		       mom[cur_num][0][1], mom[cur_num][0][2]);
	      /* Missing Values */
	      if ((num->attrs[cur_num].nr_unknown) > 0) {
		fprintf (dom_fp, "{?;%5.3f, [U,%.4f,%.4f]}\n", 
			 (double ) num->attrs[cur_num].nr_unknown/nr_exs,
			 mom[cur_num][0][1], mom[cur_num][0][2]);
	      }
	      else {
		fprintf (dom_fp, "\n");
	      }
	    }
	  }
	  break;
	}
      default : 
	{
	  fprintf (dom_fp, "\n#ERROR: unecpected type of attribute %s\n", global->attrs[i].name);
	}
      }
    }

    fflush (dom_fp);
  }
  
  
  if (rule_fp != NULL) {
    /* And now save the rule file */
    
    fprintf (rule_fp, "# DCT (Version %1.1f)  %s\n", 
	     VERSION, ctime(&hora));
    fprintf (rule_fp, "# -------------------------------------------------------------- #\n");
    fprintf (rule_fp, "# Rule File for '%s'\n#\n", name);
    fprintf (rule_fp, "#   nr_exs:   %8ld\t\t", nr_exs);
    if (NR_UNKNOWN > 0) {
      fprintf (rule_fp, "(missing: %6d/%ld)", NR_UNKNOWN, nr_exs*nr_attrs);
    }
    fprintf (rule_fp, "\n#   nr_attrs: %8d\n", nr_attrs);
    fprintf (rule_fp, "# -------------------------------------------------------------- #\n");
    fprintf (rule_fp, "\n");
    fflush (rule_fp);
    
    if (sym->nr_attrs > 1) {

      values = (char **) sym->attrs[sym->nr_attrs].vals;
      sum = 0.0;
    
      for (i=1; i<=sym->attrs[sym->nr_attrs].nr_vals; i++) {
	fprintf (rule_fp, "\n\n# Rule %2d:\n", i);
	fprintf (rule_fp, "(%6.2f %%)  %s\t<-\t", 
		 (double) dist[sym->nr_attrs][0][i]*100/nr_exs,
		 values[i]);
	fflush (rule_fp);
	sum += (double) (dist[sym->nr_attrs][0][i]*100/nr_exs);

	/* --- */
	for (j=1; j<sym->nr_attrs; j++) {
	  fprintf (rule_fp, "%s = (", sym->attrs[j].name);
	  fflush (rule_fp);
	  sym_vals = (char **) sym->attrs[j].vals;
	  sum2 = 0.0;
	  sum3 = 0;
	  first_value = TRUE;
	  
	  for (l=1; l<=sym->attrs[j].nr_vals; l++) {
	    if (((double) dist[j][i][l]/dist[sym->nr_attrs][0][i]) > 
		(0.0 + EPSILON)) {
	      if (first_value) {
		first_value = FALSE;
	      }
	      else {
		sum3 += attr_value;
		fprintf (rule_fp, "%s;%6.4f, ", attr_name, (double) attr_value / 10000.0);
	      }
	      sum2 += (double) dist[j][i][l]/dist[sym->nr_attrs][0][i];
/* 	      attr_value = (int) ((double) dist[j][i][l]/dist[sym->nr_attrs][0][i] * 10000); */
	      if (modf( ((double) dist[j][i][l]/dist[sym->nr_attrs][0][i] * 10000.0), 
			     & tmp_value) < 0.5) {
		attr_value = (int) tmp_value;
	      }
	      else {
		attr_value = (int) tmp_value + 1;
	      }
	      
	      attr_name = sym_vals[l];
	    }
	  }
	  if (((double) dist[j][i][l]/dist[sym->nr_attrs][0][i]) > 
	      (0.0 + EPSILON)) {
	    sum3 += attr_value;
	    fprintf (rule_fp, "%s;%6.4f, ", attr_name, (double) attr_value / 10000.0);
	    
	    sum2 += (double) dist[j][i][l]/dist[sym->nr_attrs][0][i];

	    attr_name = unknown_str;
/* 	    attr_value = (int) ((double) dist[j][i][l]/dist[sym->nr_attrs][0][i] * 10000); */
	    if (modf( ((double) dist[j][i][l]/dist[sym->nr_attrs][0][i] * 10000.0), 
			     & tmp_value) < 0.5) {
	      attr_value = (int) tmp_value;
	    }
	    else {
	      attr_value = (int) tmp_value + 1;
	    }
	  }

	  if ((sum2 < 1.0 - EPSILON) || (sum2 > 1.0 + EPSILON)) {
	    fprintf (rule_fp, "%s;%6.4f)", attr_name, (double) attr_value / 10000.0);
	    if (j<sym->nr_attrs-1) {
	      fprintf (rule_fp, " & \\");
	    }
	    fprintf (rule_fp, "\n# warning: default activations don't sum up to 1.0 (sum: %f)", sum2);
	    if (j<sym->nr_attrs-1) {
	      fprintf (rule_fp, "\n\t\t\t");
	    }
	  }
	  else {
	    diff = 10000 - sum3 - attr_value;
	    attr_value = (10000 - sum3);
	    fprintf (rule_fp, "%s;%6.4f)", attr_name, (double) attr_value / 10000);
	    if (j<sym->nr_attrs-1) {
	      fprintf (rule_fp, " & \\");
	    }

	    /* parser of generator can't read comments inside a rule */
/* 	    if (diff != 0) { */
/* 	      fprintf(rule_fp, "\n# warning: activation of last attribute corrected by %7.4f", */
/* 		      (double) diff / 10000.0); */
/* 	    } */
	    if (j<sym->nr_attrs-1) {
	      fprintf (rule_fp, "\n\t\t\t");
	    }
	  }
	  
/* 	  if (j<sym->nr_attrs-1) { */
/* 	    fprintf (rule_fp, " & \\ \n\t\t\t"); */
/* 	    fflush (rule_fp); */
/* 	  } */
	}

      }
    }
    else {
      fprintf (rule_fp, "# Sorry there were no symbolic attributes (except class attribute)\n");
    }
    
    fflush (rule_fp);
  }

  if (covar_fp != NULL) {
    
    fprintf (covar_fp, "# DCT (Version %1.1f)  %s\n", 
	     VERSION, ctime(&hora));
    fprintf (covar_fp, "# -------------------------------------------------------------- #\n");
    fprintf (covar_fp, "# Covariance File for '%s'\n#\n", name);
    fprintf (covar_fp, "#   nr_exs:   %8ld\t\t", nr_exs);
    if (NR_UNKNOWN > 0) {
      fprintf (covar_fp, "(missing: %6d/%ld)", NR_UNKNOWN, nr_exs*nr_attrs);
    }
    fprintf (covar_fp, "\n#   nr_attrs: %8d\n", nr_attrs);
    fprintf (covar_fp, "# -------------------------------------------------------------- #\n");
    fprintf (covar_fp, "\n");

    if (num->nr_attrs > 1) {
    
      fprintf (covar_fp, "# Attribute names:\n");
      for (i=1; i<num->nr_attrs-1; i++) {
	fprintf (covar_fp, "%s, ", num->attrs[i].name);
      }
      fprintf (covar_fp, "%s\n", num->attrs[i].name);
      fflush (covar_fp);

      fprintf (covar_fp, "\n# Means:\n");      
      for (i=1; i<num->nr_attrs-1; i++) {
	fprintf (covar_fp, "%10.4f, ", mom[i][0][3]);
      }
      fprintf (covar_fp, "%10.4f\n", mom[i][0][3]);
      fflush (covar_fp);
      
      fprintf (covar_fp, "\n# Covariances:\n");      
      for (i=1; i<num->nr_attrs; i++) {
	for (j=1; j<num->nr_attrs-1; j++) {
	  fprintf (covar_fp, "%10.4f, ", 
		   covar[0][i][j]);
	}
	fprintf (covar_fp, "%10.4f\n", 
		   covar[0][i][j]);
      }
      fprintf (covar_fp, "\n");
      fflush (covar_fp);
      
    }

    fflush (covar_fp);
  }


} /* save_datadict_to_file */



/* hack for non-ansi function rint */

double rint(double num) {
	double tmp_num;
	if (modf (num, &tmp_num) < 0.5) {
		return tmp_num;
	} 
	else {
		return tmp_num + 1.0;
	}
}

