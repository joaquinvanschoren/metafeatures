/*
 * $Source /user1/grimmer/clem/src/achim/src/RCS/results.c,v $
 * $Author: johann $
 * $Date: 2002/06/30 18:02:16 $
 * $Revision: 1.1.1.1 $
 */
/*
 *  $Log: results.c,v $
 *  Revision 1.1.1.1  2002/06/30 18:02:16  johann
 *  Initial CVS version
 *
 *  Revision 4.2  1998/10/26 15:34:16  spuhl
 *  *** empty log message ***
 *
 * Revision 4.1  1998/07/17  11:54:34  kleiner
 * switching to revision 4.1
 *
 * Revision 3.2  1998/07/02  08:25:33  spuhl
 * *** empty log message ***
 *
 * Revision 3.1  1998/06/29  07:54:57  spuhl
 * *** empty log message ***
 *
 * Revision 1.2  1998/06/29  07:01:49  spuhl
 * added for saving results in single file
 *
 */



#include <stdio.h>
#include <stdlib.h>
#include "framework.h"
#include "results.h"

double results[NR_RESULTS];
int v_calculated = TRUE;
int m_calculated = TRUE;
int bhep_calculated = FALSE;

int WriteResultFile (FILE *ResFp, char *ResFileName, char *DataSetName, int LongOut) 
{
  char *ResText[] = {
    "Nr. Attributs", 
    "Nr. symbolic Attributs",
    "Nr. numeric Attributs",
    "Nr. Tupels in Dataset",
    "Nr. Classes",
    "Accuracy of default Class",
    "Standarddeviation of Classes",
    "Minimum Number of symbolic Attributes",
    "Maximum Number of symbolic Attributes",
    "Average Number of symbolic Attributes",
    "Missing Values (total)",
    "Tupels with Missing Values",
    "Mean Skew",
    "Mean Kurtosis",
    "Attributs with Outliers",
    "Result of BHEP-Test",
    "Critical Value (Alpha = 0.1)",
    "Critical Value (Alpha = 0.05)",
    "Critical Value (Alpha = 0.01)",
    "Result of M-Statistic",
    "Degrees of Freedom",
    "Chi-Square-Distribution (M-Statistic)",
    "Result of Sd-Ratio",
    "Fract1",
    "Cancor1",
    "Result of Wilks Lambda",
    "Result of Bartlett Statistic",
    "Chi-Square-Distribution (V-Statistic)",
    "Nr.Discriminant Functions",
    "Class Entropy",
    "Entropy of Attributes",
    "Mutual Information",
    "Joint Entropy",
    "Equivalent nr. of Attributs",
    "Noise Signal Ratio"
  };
  
  int statist_fields[] = {
    NR_ATTR, NR_SYM_ATTR, NR_NUM_ATTR, NR_TUPEL, NR_TARGET_CL, \
    DEF_ACCURACY, STD_DEV_CL, MISS_VAL, MISS_VAL_LINES
  }; 

  int numeric_fields[] = {
    MEAN_SKEW, MEAN_KURTOSIS, OUTLIERS
  };

  int bhep_fields[] = {
    BHEP_RESULT, BHEP_01, BHEP_005, BHEP_001
  }; 
  
  int mstat_fields[] = {
    M_STATISTIC, DEGR_FREE, M_STAT_CHI, SD_RATIO 
  };

  int vstat_fields[] = {
    FRACT1, CANCOR1, WILKS_LAMBDA, BARTLETT, V_STAT_CHI, \
    NR_DISC_FKTN
  };

  int symbolic_fields[] = {
    SYM_CL_MIN, SYM_CL_MAX, SYM_CL_AVG, CLASS_ENTR, ENTR_OF_ATTR, \
    MUT_INFO, JOINT_ENTR, EQUI_NR_ATTR, NOISE_SIG_RAT
  }; 

  int i, field;


  if (LongOut) {
    
    if (fprintf(ResFp, "Results of Dataset %s  (%s)\n\n", ResFileName, DataSetName) < 0) {
      return 1;
    }
    
    if (fprintf(ResFp, "Overall Results Dataset %s\n", ResFileName) < 0) {
      return 1;
    }
    for (i = 0; i < (int) (sizeof(statist_fields)/sizeof(int)); i++) {
      field = statist_fields[i];
      if (fprintf(ResFp, "%s : %f\n", ResText[field], results[field]) < 0) {
	return 1;
      }
    }
    
    if (fprintf(ResFp, "\n Results of numerical fields\n") < 0) {
      return 1;
    }
    if (results[NR_NUM_ATTR] > 0) {
      for (i = 0; i < (int) (sizeof(numeric_fields)/sizeof(int)); i++) 
	  {
		field = numeric_fields[i];
		if (quant_calculated || field!=OUTLIERS)
		{
			if (fprintf(ResFp, "%s : %f\n", ResText[field], results[field]) < 0) 
			{
				return 1; 
			}
		}
		else
			if (fprintf(ResFp, "%s : *\n", ResText[field]) < 0) 
			{
				return 1;
			}
		
	  }
    }
    else {
      for (i = 0; i < (int) (sizeof(numeric_fields)/sizeof(int)); i++) {
	field = numeric_fields[i];
	if (fprintf(ResFp, "%s : *\n", ResText[field]) < 0) {
	  return 1;
	}
      }
    } 
    
    if (fprintf(ResFp, "\n Results of BHEP-Test\n") < 0) {
      return 1;
    }
    if (bhep_calculated) {
      for (i = 0; i < (int) (sizeof(bhep_fields)/sizeof(int)); i++) {
	field = bhep_fields[i];
	if (fprintf(ResFp, "%s : %f\n", ResText[field], results[field]) < 0) {
	  return 1;
	}
      }
    }
    else {
      for (i = 0; i < (int) (sizeof(bhep_fields)/sizeof(int)); i++) {
	field = bhep_fields[i];
	if (fprintf(ResFp, "%s : *\n", ResText[field]) < 0) {
	  return 1;
	}
      }
    }
    
    if (fprintf(ResFp, "\n Results of M-Statistic\n") < 0) {
      return 1;
    }
    if (m_calculated) {
      for (i = 0; i < (int) (sizeof(mstat_fields)/sizeof(int)); i++) {
	field = mstat_fields[i];
	if (fprintf(ResFp, "%s : %f\n", ResText[field], results[field]) < 0) {
	  return 1;
	}
      }
    }
    else {
      for (i = 0; i < (int) (sizeof(mstat_fields)/sizeof(int)); i++) {
	field = mstat_fields[i];
	if (fprintf(ResFp, "%s : *\n", ResText[field]) < 0) {
	  return 1;
	}
      }
    }
    
    if (fprintf(ResFp, "\n Results of V-Statistic\n") < 0) {
      return 1;
    }
    if (v_calculated) {
      for (i = 0; i < (int) (sizeof(vstat_fields)/sizeof(int)); i++) {
	field = vstat_fields[i];
	if (fprintf(ResFp, "%s : %f\n", ResText[field], results[field]) < 0) {
	  return 1;
	}
      }
    }
    else {
      for (i = 0; i < (int) (sizeof(vstat_fields)/sizeof(int)); i++) {
	field = vstat_fields[i];
	if (fprintf(ResFp, "%s : *\n", ResText[field]) < 0) {
	  return 1;
	}
      }
    }
    
    if (fprintf(ResFp, "\n Results of symbolic Fields\n") < 0) {
      return 1;
    }
    if (results[NR_SYM_ATTR] > 0 && entr_calculated) {
      for (i = 0; i < (int) (sizeof(symbolic_fields)/sizeof(int)); i++) {
	field = symbolic_fields[i];
	if (fprintf(ResFp, "%s : %f\n", ResText[field], results[field]) < 0) {
	  return 1;
	}
      }
    }
    else {
      for (i = 0; i < (int) (sizeof(symbolic_fields)/sizeof(int)); i++) {
	field = symbolic_fields[i];
	if (field!=CLASS_ENTR || !entr_calculated) {
		if (fprintf(ResFp, "%s : *\n", ResText[field]) < 0) {
		return 1;
		}
	} else
		if (fprintf(ResFp, "%s : %f\n", ResText[field], results[field]) < 0) {
		return 1;
		}
	  }
    }
  }
  else {
    if (fprintf(ResFp, "%s\n", ResFileName) < 0) {
      return 1;
    }
    for (i = 0; i < (int) (sizeof(statist_fields)/sizeof(int)); i++) {
      field = statist_fields[i];
      if (fprintf(ResFp, "%f\n", results[field]) < 0) {
	return 1;
      }
    }
    
    if (results[NR_NUM_ATTR] > 0) {
      for (i = 0; i < (int) (sizeof(numeric_fields)/sizeof(int)); i++) {
	field = numeric_fields[i];
	
	if (quant_calculated || field!=OUTLIERS) {
		if (fprintf(ResFp, "%f\n", results[field]) < 0) {
			return 1; } }
	else
		if (fprintf(ResFp, "*\n") < 0) {
			return 1;
			}
      }
    }
    else {
      for (i = 0; i < (int) (sizeof(numeric_fields)/sizeof(int)); i++) {
	field = numeric_fields[i];
	if (fprintf(ResFp, "*\n") < 0) {
	  return 1;
	}
      }
    } 
    
    if (bhep_calculated) {
      for (i = 0; i < (int) (sizeof(bhep_fields)/sizeof(int)); i++) {
	field = bhep_fields[i];
	if (fprintf(ResFp, "%f\n", results[field]) < 0) {
	  return 1;
	}
      }
    }
    else {
      for (i = 0; i < (int) (sizeof(bhep_fields)/sizeof(int)); i++) {
	field = bhep_fields[i];
	if (fprintf(ResFp, "*\n") < 0) {
	  return 1;
	}
      }
    }
    
    if (m_calculated) {
      for (i = 0; i < (int) (sizeof(mstat_fields)/sizeof(int)); i++) {
	field = mstat_fields[i];
	if (fprintf(ResFp, "%f\n", results[field]) < 0) {
	  return 1;
	}
      }
    }
    else {
      for (i = 0; i < (int) (sizeof(mstat_fields)/sizeof(int)); i++) {
	field = mstat_fields[i];
	if (fprintf(ResFp, "*\n") < 0) {
	  return 1;
	}
      }
    }
    
    if (v_calculated) {
      for (i = 0; i < (int) (sizeof(vstat_fields)/sizeof(int)); i++) {
	field = vstat_fields[i];
	if (fprintf(ResFp, "%f\n", results[field]) < 0) {
	  return 1;
	}
      }
    }
    else {
      for (i = 0; i < (int) (sizeof(vstat_fields)/sizeof(int)); i++) {
	field = vstat_fields[i];
	if (fprintf(ResFp, "*\n") < 0) {
	  return 1;
	}
      }
    }
    
    if (results[NR_SYM_ATTR] > 0 && entr_calculated) {
      for (i = 0; i < (int) (sizeof(symbolic_fields)/sizeof(int)); i++) {
	field = symbolic_fields[i];
	if (fprintf(ResFp, "%f\n", results[field]) < 0) {
	  return 1;
	}
      }
    }
    else {
      for (i = 0; i < (int) (sizeof(symbolic_fields)/sizeof(int)); i++) {
	field = symbolic_fields[i];
	
	if (field!=CLASS_ENTR || !entr_calculated) {
		if (fprintf(ResFp, "*\n") < 0) {
		return 1;
		}
	} else
		if (fprintf(ResFp, "%f\n", results[field]) < 0) {
		return 1;
		}
	  
	  }
    }
  }
    

  fflush(ResFp); 
  return 0;
}


void InitResults() {
  int i;

  for (i = 0; i < NR_RESULTS; results[i++] = 0);
}
