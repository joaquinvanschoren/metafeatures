/*
 * $Source /user1/grimmer/clem/src/achim/src/RCS/results.h,v $
 * $Author: johann $
 * $Date: 2002/06/30 18:02:16 $
 * $Revision: 1.1.1.1 $
 */
/*
 *  $Log: results.h,v $
 *  Revision 1.1.1.1  2002/06/30 18:02:16  johann
 *  Initial CVS version
 *
 *  Revision 4.2  1998/10/26 15:33:45  spuhl
 *  rm *.o
 *  ll
 *
 * Revision 4.1  1998/07/17  11:54:32  kleiner
 * switching to revision 4.1
 *
 * Revision 3.2  1998/07/02  08:25:31  spuhl
 * *** empty log message ***
 *
 * Revision 3.1  1998/06/29  07:54:19  spuhl
 * *** empty log message ***
 *
 * Revision 1.2  1998/06/29  07:01:21  spuhl
 * added for saving results in single file
 *
 */



#define NR_ATTR         0
#define NR_SYM_ATTR     1
#define NR_NUM_ATTR     2
#define NR_TUPEL        3
#define NR_TARGET_CL    4
#define DEF_ACCURACY    5
#define STD_DEV_CL      6
#define SYM_CL_MIN      7
#define SYM_CL_MAX      8 
#define SYM_CL_AVG      9
#define MISS_VAL       10
#define MISS_VAL_LINES 11
#define MEAN_SKEW      12 
#define MEAN_KURTOSIS  13
#define OUTLIERS       14
#define BHEP_RESULT    15
#define BHEP_01        16
#define BHEP_005       17
#define BHEP_001       18
#define M_STATISTIC    19
#define DEGR_FREE      20
#define M_STAT_CHI     21
#define SD_RATIO       22
#define FRACT1         23
#define CANCOR1        24
#define WILKS_LAMBDA   25
#define BARTLETT       26
#define V_STAT_CHI     27
#define NR_DISC_FKTN   28
#define CLASS_ENTR     29
#define ENTR_OF_ATTR   30
#define MUT_INFO       31
#define JOINT_ENTR     32
#define EQUI_NR_ATTR   33
#define NOISE_SIG_RAT  34

#define NR_RESULTS     35


extern double results[NR_RESULTS];
extern int v_calculated;
extern int m_calculated;
extern int bhep_calculated;
extern int entr_calculated;
extern int quant_calculated;
extern int mcorr_calculated;

int WriteResultFile (FILE *ResFp, char *ResFileName, char *DataSetName, int LongOut);
void InitResults();





