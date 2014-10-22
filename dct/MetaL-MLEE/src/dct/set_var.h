/*
 * $Source /user1/grimmer/clem/src/achim/src/RCS/set_var.h,v $
 * $Author: johann $
 * $Date: 2002/06/30 18:02:16 $
 * $Revision: 1.1.1.1 $
 */
/* 
 *  $Log: set_var.h,v $
 *  Revision 1.1.1.1  2002/06/30 18:02:16  johann
 *  Initial CVS version
 *
 *  Revision 4.1  1998/07/17 11:53:44  kleiner
 *  switching to revision 4.1
 *
 * Revision 3.3  1998/07/02  08:24:41  spuhl
 * *** empty log message ***
 *
 * Revision 3.2  1998/06/29  06:37:50  spuhl
 * *** empty log message ***
 *
 * Revision 3.1  1998/06/23  12:41:41  kleiner
 * *** empty log message ***
 *
 * Revision 2.6  1998/06/22  12:32:08  kleiner
 * added CHI_GAMMA (significance level for chi^2-calculations)
 *
 * Revision 2.5  1998/06/12  12:25:36  thor
 * added EPSILON (used by save_datadict_to_file)
 *
 *  Revision 2.4  1998/06/04 10:50:18  kleiner
 *  added CONF_ALPHA
 *
 * Revision 2.3  1998/05/18  07:04:11  kleiner
 * *** empty log message ***
 *
 * Revision 2.2  1998/05/17  17:39:28  Administrator
 * *** empty log message ***
 *
 *  Revision 2.1  1998/05/15 07:13:47  kleiner
 *  *** empty log message ***
 *
 * Revision 1.2  1998/05/07  16:19:11  kleiner
 * *** empty log message ***
 *
 */

#include <stdio.h>


#define EPSILON		0.0000000001 /* TK */
/* TK */
#define CONF_ALPHA	0.95
#define CHI_GAMMA	0.95

#define BHEP_ALPHA 	0.1		/* value for BHEP-test	*/
#define NR_OF_VAL  	10 		/* in the table         */
#define BHEP_BETA	1.0		/* value for BHEP-test	*/
#define LOW_QUANT	0.25		/* low quantile		*/
#define HIGH_QUANT	0.75		/* high quantile	*/
#define MED_QUANT	0.5		/* median quantile	*/
#define GETRIM_ALPHA	0.05		/* value for alpha-getri*/
#define ALPHA           0.05            /* value for a measure  */ 
#define OUT_LOW         0.7             /* outliers             */ 
#define CONST           0.5             /* value for g-function */
#define LOG2(x)		log(x)/log(2)   /* computes log_2(x)    */ 
#define NR_CATEG_EX     6               /* nr. of categories for (nr of examples)  */  


/* (TK):
 *
 *  the following definitions used to be static.
 *  changed to extern. 
 *  the actual definition is done by 'analist.c' (as globals)
 */
extern int nr_of_examples[6]; 
extern char nr_of_categories[6]; 
extern double alpha_phi[10][2]; 

/* (TK): definition, taken from helpfunc.c to avoid warnings */
extern void get_time(char *test);

