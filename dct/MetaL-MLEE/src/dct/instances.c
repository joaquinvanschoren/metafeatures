/*
 * $Source /user1/grimmer/clem/src/achim/src/RCS/instances.c,v $
 * $Author: johann $
 * $Date: 2002/06/30 18:02:16 $
 * $Revision: 1.1.1.1 $
 */
/*
 *  $Log: instances.c,v $
 *  Revision 1.1.1.1  2002/06/30 18:02:16  johann
 *  Initial CVS version
 *
 *  Revision 4.2  1998/07/21 12:20:08  spuhl
 *  mehods to filter "\" added
 *
 * Revision 4.1  1998/07/17  11:53:25  kleiner
 * switching to revision 4.1
 *
 * Revision 3.5  1998/07/15  08:15:35  spuhl
 * new function ReadAttribute (before ReadField)
 *
 * Revision 3.4  1998/07/07  16:45:08  spuhl
 * changed sort-functions to fix qsort warning
 *
 * Revision 3.3  1998/07/02  17:05:39  spuhl
 * fixed german output
 * fixed output of unknown values
 *
 * Revision 3.2  1998/07/02  08:24:29  spuhl
 * *** empty log message ***
 *
 * Revision 3.1  1998/06/23  12:41:35  kleiner
 * *** empty log message ***
 *
 * Revision 2.8  1998/06/06  17:07:21  thor
 * missing values!
 *
 *  Revision 2.7  1998/05/25 16:04:37  kleiner
 *  *** empty log message ***
 *
 * Revision 2.6  1998/05/20  10:46:13  kleiner
 * *** empty log message ***
 *
 * Revision 2.5  1998/05/19  15:28:58  kleiner
 * strtol
 *
 * Revision 2.4  1998/05/18  07:04:03  kleiner
 * *** empty log message ***
 *
 * Revision 2.3  1998/05/17  17:39:16  Administrator
 * *** empty log message ***
 *
 *  Revision 2.2  1998/05/15 14:23:26  kleiner
 *  *** empty log message ***
 *
 * Revision 2.1  1998/05/15  07:13:40  kleiner
 * *** empty log message ***
 *
 * Revision 1.6  1998/05/14  18:08:52  thor
 * *** empty log message ***
 *
 *  Revision 1.5  1998/05/14 16:40:21  kleiner
 *  avoided recursion
 *  removed rlimit
 *
 * Revision 1.4  1998/05/14  13:35:09  kleiner
 * added rlimit...
 *
 * Revision 1.3  1998/05/08  11:19:54  kleiner
 * added some output for ReadAttvals (in case of error)
 *
 * Revision 1.2  1998/05/07  16:19:04  kleiner
 * *** empty log message ***
 *
 * 08.05.98  kleiner: added some output (ReadAttVal)
 */

/*********************************************************************/
/*        FRAME WORK FOR PROPOSITIONAL LEARNING                      */
/*        Versao 1.0 (10/12/1996)     FILE:instances.c               */
/*  Developed by: Joao Gama, Luis Torgo                              */
/*                LIACC - Uni.do Porto                               */
/*                {jgama, ltorgo}@ncc.up.pt                          */
/*********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "framework.h"
#include "set_var.h"
#include <errno.h>
#include "measures.h" /* US added */
extern int PROGINFO;
extern void print_line ();

typedef struct {
  int     att;
  int     nr_val;
  List    *values;
} _A_V;

#define AV_ATT(av)        av->att
#define AV_VALUES(av)     av->values;
List *bag = NULL;
char temp[_MAX_SIZE_LINE_];

List *search_position(char *value,  _A_V *av, int *pos);
List *bag_values(List *bag, int att, char *value, int *pos);
AttrVal **_ReadInstances(FILE *fi, DomainInfo *domain, unsigned long 
*nrexs, unsigned long nr);
AttrVal *ReadAttVal(FILE *fi, DomainInfo *domain, unsigned long nrex);
char *ReadAttribute(FILE *fi, char *sep);
int A_V_att(_A_V *av);
void update_domain(List *bag, DomainInfo *domain);
/* int compare_chars(char **s1, char **s2); */
/* US declared in measures.h
int compare_chars(const void **s1, const void **s2); */
void WriteInstanceToFile(FILE *, DomainInfo *, AttrVal *, char *, char 
*);


char    UNKNOWN  = '?'; 
char    DONTCARE = '*'; 


/* ######################################################################
   METHODS SPECIFIC TO INSTANCES STORED AS A LIST OF ARRAYS
   ====================================================================== */


/* ======================================================================
   READING THE INSTANCES FROM A FILE.
   ---------------------------------------------------------------------- */
List *LReadInstances(FILE *fi, DomainInfo *domain, unsigned long *nrexs)
{
  static unsigned long  nr = 1;
  AttrVal      *instance;

  if (feof(fi)) {
    update_domain(bag, domain);
    *nrexs = nr - 1;
    return NULL;
  }
  if ((instance = ReadInstance(fi, domain, nr)) != NULL){
    nr++;
    return (insert(instance, LReadInstances(fi, domain, nrexs)));
  }
  return LReadInstances(fi, domain, nrexs);
}



/* ======================================================================
   SHOWING A LIST OF INSTANCES (A DATASET).
   ---------------------------------------------------------------------- */
void Show_LInstances(DomainInfo *domain, List *instances, int low, int high)
{
  register int i = 1;

  for (i = 1; i < low; i++)     instances = tail(instances);
  for(; i <= high; i++) {
    ShowInstance(domain, head(instances));
    instances = tail(instances);
  }
}

/* ######################################################################
   METHODS SPECIFIC TO INSTANCES STORED AS AN ARRAY OF ARRAYS
   ====================================================================== */

/* ======================================================================
   READING THE INSTANCES FROM A FILE.
   ---------------------------------------------------------------------- */
Dataset *
ReadDataset(FILE *fi, DomainInfo *domain)
{
  Dataset *ds = (Dataset *) malloc(sizeof(Dataset));
  if (ds) {
    ds->instances = _ReadInstances(fi, domain, &ds->nr_exs, 1);
    ds->domain = domain;
  }
  else
    fprintf(stderr, "ReadDataset: Out of memory\n");
  return ds;
}

AttrVal **
_ReadInstances(FILE *fi, DomainInfo *domain, unsigned long 
	       *nrexs, unsigned long nr)
{
  AttrVal *instance = NULL, 
  **instances = (AttrVal **) malloc (sizeof(AttrVal*));
  char text[200];

  *nrexs = 1;

  while (! feof(fi)) {
    /* TK: */
    if ((instances = (AttrVal **) realloc(instances,(*nrexs+1) * sizeof(AttrVal *))) == NULL) {
      fprintf(stderr, "ReadInstances: Not enough memory\n");
      exit(1);
    }
    
    if ((instance = ReadInstance(fi, domain, *nrexs)) != NULL) {
      instances[*nrexs] = instance;
      if ((PROGINFO) && ((*nrexs % 1000) == 0)) { /* TK */
	sprintf (text, "read %6ld rows", *nrexs);
	get_time (text);
      }
      *nrexs += 1;
    }
  }

  *nrexs -= 1;
  
  if (PROGINFO) {
    sprintf (text, "read %6ld rows total!", *nrexs);
    get_time (text);
  }

  return instances;

  /* TK END */

}


/* ======================================================================
   SHOWING A LIST OF INSTANCES (A DATASET).
   ---------------------------------------------------------------------- */
void 
Show_Instances(Dataset *ds, int low, int high) {
  register unsigned long i;

  if (high > ds->nr_exs) high = ds->nr_exs;
  for (i = low; i <= high; i++)
    ShowInstance(ds->domain, ds->instances[i]);
}


/* ######################################################################
   METHODS INDEPENDENT OF THE DATA STRUCTURE USED FOR STORING INSTANCES
   ====================================================================== */


/* ======================================================================
   READING ONE INSTANCE.
   ---------------------------------------------------------------------- */
AttrVal *ReadInstance(FILE *fi, DomainInfo *domain, unsigned long nrex)
{
  char      *value = NULL;
  AttrVal   *instance = NULL;

  if ((instance = ReadAttVal(fi, domain, nrex)) != NULL) {
    while ((value = ReadAttribute(fi, ",\t ")) != NULL);
  }

  return instance;
}


/* ======================================================================
   SHOWING ONE INSTANCE.
   ---------------------------------------------------------------------- */
void ShowInstance(DomainInfo *domain, AttrVal *instance)
{
  register int j;
  
  if (instance != NULL) {
    for(j = 1; j <= NrAttrs(domain); j++) {
      switch(TypeOfVal(instance[j])) {
      case normal:
	switch(TypeAttr(domain, j)) {
	case continuous: 
          printf("%7.3f\t", instance[j].val.c);
	  break;
	case integer:
	  printf("%7d\t", instance[j].val.d);
	  break;
	case structured:
	case ordered:
	case nominal:
          printf("%7s\t", LblValId(domain, j, instance[j].val.d));
	  break;
	}
	break;
      case unknown:
        printf("%7s\t", "?");
	break;
      case dontcare:
        printf("%7s\t", "?");
	break;
      }
    }
    printf("\n");
  }
}



/*************************************************** 
   Goal:  Write a set of instances between [Low .. High] on a file

   Input: Name      - Output File Name
          domain    - Domain Info
          instances - Array of arrays of instances
	  low, high - Limits of instances
	  SepFields - Character field separator
	  SepRec    - Character record separator
   Output:  TRUE if succeds  FALSE otherwise
****************************************************/
int _WriteToFile\
(FILE *fo, Dataset *ds, unsigned long low, unsigned long high, char *SepFields, 
char *SepRec)
{
  register unsigned long i;

  if (high > ds->nr_exs)  high = ds->nr_exs;
  for(i = low; i <= high; i++)   
    WriteInstanceToFile(fo, ds->domain, ds->instances[i],SepFields,SepRec);
  return TRUE;
}

void WriteInstanceToFile\
(FILE *fi, DomainInfo *domain, AttrVal *instance, char *SepFields, char *SepRec)
{
  register int j;
  
  if (instance != NULL) {
    for(j = 1; j <= NrAttrs(domain); j++) {
      switch(TypeOfVal(instance[j])) {
      case normal:
	switch(TypeAttr(domain, j)) {
	case continuous: 
          fprintf(fi, "%6.3f%c", instance[j].val.c, *SepFields);
	  break;
	case integer:
	  fprintf(fi, "%d%c", instance[j].val.d, *SepFields);
	  break;
	case structured:
	case ordered:
	case nominal:
          fprintf(fi, "%s%c", LblValId(domain, j, instance[j].val.d), 
*SepFields);
	  break;
	}
	break;
      case unknown:
        fprintf(fi, "?%c", *SepFields);
	break;
      case dontcare:
        fprintf(fi, "?%c", *SepFields);
	break;
      }
    }
    fprintf(fi,"%c", *SepRec);
  } 
}


/***********************************************************/
/*    Private Methods:  Instances                          */
/***********************************************************/
AttrVal *ReadAttVal(FILE *fi, DomainInfo *domain, unsigned long nrex)
{
  register int    i;
  int             pos = 1;
  char            *value = NULL;
  AttrVal         *attr_val = (AttrVal *) calloc(1+NrAttrs(domain), sizeof(AttrVal));
  int		  missing = FALSE; /* TK */
  extern int	  NR_LINES_UNKNOWN; /* TK */
  extern int	  NR_UNKNOWN; /* TK */

  char **endptr = (char **) malloc (sizeof (char *));


  if (fi == NULL) {
    fprintf (stderr, "***(901)READATTVAL: fi is NULL\n");
    fflush (stderr);
    exit (901);
  }
  if (domain == NULL) {
    fprintf (stderr, "***(902)READATTVAL: domain is NULL\n");
    fflush (stderr);
    exit (902);
  }

 
  /* ++line_nr; */


  if (attr_val == NULL) {
    fprintf(stderr, "ReadAttVal: Out of Memory\n");
    return NULL;
  }

  Id(attr_val) = (ContType) nrex;
  for(i = 1; i <= NrAttrs(domain) && pos != -1; i++) {
    if ((value = ReadAttribute(fi, ",\t ")) != NULL) {
      if (*value == UNKNOWN) {
	TypeOfVal(attr_val[i]) = unknown;
	domain->attrs[i].nr_unknown += 1;
	NR_UNKNOWN++;
	missing = TRUE;
      }
      else if (*value == DONTCARE) TypeOfVal(attr_val[i]) = dontcare;
      else {  
	switch(TypeAttr(domain, i)) {
	case continuous: 
	  errno = 0;
	  attr_val[i].val.c = strtod (value, (char **) endptr);
	  if ((value == endptr[0]) || (errno != 0)) {
	    if (errno == ERANGE) {
	      fprintf (stderr, "ReadAttVal (903): Out of range!\n");
	    }
	    fprintf (stderr, "ReadAttVal: found '%s' expecting continuous (line: %ld attribute: %s)\n", 
		     value, nrex, domain->attrs[i].name);
	    fflush (stdout);
	    fflush (stderr);
	    exit (903);
	    return NULL;
	  }
	  /* attr_val[i].val.c =  atof(value); */
	  break;
	case integer:
	  errno = 0;
	  attr_val[i].val.d = strtol (value, (char **) endptr, 10);
	  if (errno == ERANGE) {
	    fprintf (stderr, "ReadAttVal (904): Out of range!\n");
	  }
	  if ((value == endptr[0]) || (errno != 0)) {
	    fprintf (stderr, "ReadAttVal: found '%s' expecting integer (line: %ld attribute: %s)\n", 
		     value, nrex, domain->attrs[i].name);
	    fflush (stdout);
	    fflush (stderr);
	    exit (904);
	    return NULL;
	  }
	  /* attr_val[i].val.d =  atoi(value); */
	  if (NValsAttr(domain, i) == 0)
	    bag = bag_values(bag, i, value, &pos);
	  break;
	case ordered:
	case nominal:
	  if (NValsAttr(domain, i) != 0) 
	    pos = IdValLbl(domain, i, value);
	  else
	    bag = bag_values(bag, i, value, &pos);
	  if (pos)
	    attr_val[i].val.d = pos;
	  else {
	    /* (TK): */
	    print_line ();
	    fprintf (stderr, "Error (999) => Showing Domain Information:\n\n");
	    fflush (stderr);
	    ShowDomain (domain);
	    ShowTarget(domain);
	    fflush (stdout);
	    fprintf(stderr, "*** ReadAttVal: Line %ld (attribute '%s'): Invalid attribute value\n", 
		    nrex, domain->attrs[i].name);
	    fprintf (stderr, "\tFound: '%s' (not in domain)\n",
		     value);
	    fflush (stderr);
	    print_line ();
	    fflush (stdout);
	    exit (999);
	  }
	  break;
	case structured:
	  /* (TK): */
	  fprintf (stderr, "*** ReadAttVal: found attribute type structured!!!\n");
	  if ((pos = IdValLbl(domain, i, value)) != 0)
	    attr_val[i].val.d = pos;
	  else {
	    fprintf(stderr, "*** ReadAttVal: Line %ld (attribute '%s'): Invalid attribute value\n", 
		    nrex, domain->attrs[i].name);
	    /* fprintf(stderr, "*** ReadAttVal: Instance %ld (Line %ld) Invalid attribute value\n", nrex, nrex);  TK: changed line_nr to nrex */
	     /* (TK): */
	     fprintf (stderr, "\t(case structured:) Found: '%s' (not in domain)\n",
		      value);
	    exit (998);
	   }
	  break;	  
	}
      }
    }
    else break;
  }


  if (missing) NR_LINES_UNKNOWN ++; /* TK */
  

  if (i > NrAttrs(domain)) return attr_val;
  if (i > 1) {
    fprintf(stderr, "*ReadAttVal: Instance %ld (Line %ld) Invalid number of attributes\n "\
	    "     Nr.Attributes: %d , found: %d\n" , nrex, nrex, NrAttrs(domain), i); /* TK: changed line_nr to nrex */
    free (attr_val);
    exit (997);
  }

  free(attr_val);
  
  return NULL;
}


/**************************************/
/*   Public Methods  for SORT         */
/**************************************/
/*************************************************** 
   Goal:  Sort instances between [Low .. High] 
          by the values of attribute Att
   Input: Domain Info
          Array of arrays of instances
	  Attribute 
	  Limits of instances
   Output:  TRUE or FALSE
****************************************************/
int QuickSort(Dataset *ds, int Att, unsigned long Low, unsigned long High)
{
  register unsigned long i, Lower, Middle;
  double Thresh, value;
  enum AttrTypes tipo;

  fprintf (stderr, "******called Quicksort!!!!!!\n");


  tipo = TypeAttr(ds->domain, Att);
  if ( Low < High ) {
    switch(tipo) {
    case continuous:
      Thresh = CValAttEx(ds->instances[Low], Att);
      break;
    case nominal:
    case ordered:
    case integer:
      Thresh = DValAttEx(ds->instances[Low], Att);
      break;
    default:
    /* otherwise: */
      return FALSE;
    }
    Middle = Low;
    for ( i = Low ; i <= High ; i++ ) {
      value = tipo == continuous ? CValAttEx(ds->instances[i],Att) : (double) 
	DValAttEx(ds->instances[i],Att);
      if (value <= Thresh )  { 
        if (i != Middle ) Exchange(ds->instances,Middle, i);
        Middle++; 
      }
    }
    if (Middle != Low) {
      Lower = Middle - 1;
      for ( i = Lower ; i >= Low ; i-- ) {
	value = tipo == continuous ? CValAttEx(ds->instances[i],Att) : (double) 
	  DValAttEx(ds->instances[i],Att);
        if (value == Thresh )  { 
          if ( i != Lower ) Exchange(ds->instances,Lower, i);
          Lower--;
        }
      }
      QuickSort(ds, Att, Low, Lower);
      QuickSort(ds, Att, Middle, High);
    }
  }
  return TRUE;
}
/*************************************************** 
   Goal:  Join all examples with the same value (nominal attributes)
   Input: Domain Info
          Array of arrays of instances
	  Attribute 
	  Spliting value
	  Limits of examples
   Output:  Spliting point
****************************************************/
int JoinValues(Dataset *ds, int Att, int value, unsigned long Low, unsigned long 
High)
{
  register unsigned long i, Pos = Low;
  
  for (i = Low ; i <= High ; i++) {
    if (DValAttEx(ds->instances[i],Att) == value) { 
      if (i != Pos ) Exchange(ds->instances, Pos, i);
      Pos++; 
    }
  }
  return Pos;
}
/*************************************************** 
   Goal:  Cut points (quantitative attributes)
   Input: Domain Info
          Array of arrays of instances
	  Attribute 
	  Limits of examples
	  Spliting value
   Output:  Spliting point
   Remarks: Assumes the examples ordered by Attribute value
****************************************************/
int SplitingPosition(Dataset *ds, int Att, unsigned long Low, unsigned long 
High, double value)
{
  register unsigned long i = Low;
  if(TypeAttr(ds->domain, Att) == continuous)
    for(i = Low; i <= High && value > CValAttEx(ds->instances[i],Att); i++) ;
  else
    for(i = Low; i <= High && value > DValAttEx(ds->instances[i],Att); i++) ;
  return i-1;
}

/*************************************************** 
   Goal:  Change positions between ex1 and ex2
   Input: Array of arrays of instances
          position of examples ex1 and ex2
   Output: void
****************************************************/
void Exchange(AttrVal **instances, unsigned long ex1, unsigned long ex2)
{
  AttrVal *aux = instances[ex1];

  instances[ex1] = instances[ex2];
  instances[ex2] = aux;
}
/**************************************
  Bagging Values of attibutes not defined on Domain
***************************************/
List *bag_values(List *bag, int att, char *value, int *pos)
{
  List *r; /* (TK): unused: , *val; */
  _A_V *av;

  r = bag;
  while(!empty(bag) && att !=  A_V_att(head(bag)))
    bag = tail(bag);
  if (empty(bag)) {
    *pos = 1;
    av = (_A_V *) malloc(sizeof(_A_V));
    av->att = att;
    av->nr_val = 1;
    av->values = insert(value, NULL);
    return insert(av, r);
  }
  av = head(bag);
  av->values = search_position(value, av, pos);
  return r;
}

int A_V_att(_A_V *av)
{
  return av->att;
}

List *search_position(char *value,  _A_V *av, int *pos)
{
  register int i = 1;
  List *vals = av->values;

  while(!empty(vals) && strcmp(value, (char *)head(vals))) {
    vals = tail(vals);
    ++i;
  }
  *pos = i;
  if (empty(vals))  {
    ++av->nr_val;
    return insert(value, av->values);
  }
  return av->values;
}

void 
update_domain(List *bag, DomainInfo *domain)
{
  register int i;
  char **values = NULL;
  List *val;
  _A_V *av;

  fprintf (stderr, "update_domain BEGIN\n"); /* TK */

  if (!empty(bag)) {
    fprintf (stderr, "doing nothing bag is empty\n");
  }

  while (!empty(bag)) {
    av = head(bag);
    val = av->values;
    values = (char **) malloc(av->nr_val * sizeof(char *));
    --values;
    for (i = 1; i <= av->nr_val; i++) {
      values[i] = (char *) head(val);
      val = tail(val);
    }
    qsort(values + 1, av->nr_val, sizeof(char *), compare_chars);
    ValsAttr(domain, av->att) = values;
    NValsAttr(domain, av->att) =  av->nr_val;
    bag = tail(bag);
  }

  fprintf (stderr, "update_domain END\n"); /* TK */

}

/* US moved to helpfunc.c, declaration in framework.h
int compare_chars(const void **s1, const void **s2)
{
  return atoi((char *) *s1) - atoi((char *) *s2);
}
*/

/* =================================================================
   THESE TWO FUNCTIONS CAN BE USED TO CHANGE THE DEFAULT CHARACTERS
   USED TO REPRESENT AN UNKNOWN AND A DONTCARE VALUE.
   ----------------------------------------------------------------- */
void SetUnknownChar(char c)
{
  UNKNOWN = c;
}

void SetDontcareChar(char c)
{
  DONTCARE = c;
}




/* =================================================================

   ================================================================= */
char *ReadAttribute(FILE *fi, char *sep)
{
  register int i = 0;
  char ch;
  int special, lastspecial;
  
  ch = *sep;
  while (contained(ch, sep))      ch = getc(fi);  /* skip Whitespaces */
  if (end_of_record(ch))          return(NULL);
  special = lastspecial = FALSE;
  
  while((!contained(ch, sep) || special) && !end_of_record(ch)) {
    if (i >= _MAX_SIZE_LINE_) {
      fprintf (stderr, "ReadAttribute: field to long (_MAX_SIZE_LINE_: %d)\n",
	       _MAX_SIZE_LINE_);
      temp[_MAX_SIZE_LINE_ - 1] = '\0';
      fprintf (stderr, "FOUND: \n%s\n", temp);
      exit (7);
    }
    if (special) {
      temp[i++] = ch;
      ch = getc(fi);
      special = FALSE;
      lastspecial = TRUE;
    }
    else {
      lastspecial = FALSE;
      if (ch == '\\') {
	special = TRUE;
	ch = getc(fi);
      }
      else {
	temp[i++] = ch;
	ch = getc(fi);
      }
    }
  }

  if (end_of_record(ch))  ungetc(ch, fi);
  if (!lastspecial && (temp[i-1] == '.')) i--;
  temp[i] = '\0';

  return temp;
}





