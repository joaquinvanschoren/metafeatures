/*
 * $Source /user1/grimmer/clem/src/achim/src/RCS/domain.c,v $
 * $Author: johann $
 * $Date: 2002/06/30 18:02:16 $
 * $Revision: 1.1.1.1 $
 */
/*
 *  $Log: domain.c,v $
 *  Revision 1.1.1.1  2002/06/30 18:02:16  johann
 *  Initial CVS version
 *
 *  Revision 4.2  1998/07/21 12:19:00  spuhl
 *  mehods to filter "\" added
 *
 * Revision 4.1  1998/07/17  11:52:56  kleiner
 * switching to revision 4.1
 * >> .
 *
 * Revision 3.6  1998/07/15  08:14:51  spuhl
 * avoided recursion
 * new functions to read Domain
 *
 * Revision 3.5  1998/07/07  16:43:08  spuhl
 * changed sort-functions to fix qsort warning
 *
 * Revision 3.4  1998/07/03  12:08:29  kleiner
 * allowed one 'attrname: value' for symbolic attributes
 *
 * Revision 3.3  1998/07/02  17:01:03  spuhl
 * fixed german output
 * fixed output of target field
 *
 * Revision 3.2  1998/07/02  08:24:18  spuhl
 * *** empty log message ***
 *
 * Revision 3.1  1998/06/23  12:41:26  kleiner
 * *** empty log message ***
 *
 * Revision 2.10  1998/06/06  17:06:56  thor
 * check analist.c
 *
 *  Revision 2.9  1998/06/05 14:37:01  kleiner
 *  *** empty log message ***
 *
 * Revision 2.8  1998/06/04  16:15:26  kleiner
 * 'isint'
 *
 * Revision 2.7  1998/05/26  12:16:30  kleiner
 * exit on error in ReadDomain
 *
 * Revision 2.6  1998/05/25  16:04:33  kleiner
 * *** empty log message ***
 *
 * Revision 2.5  1998/05/20  10:46:09  kleiner
 * *** empty log message ***
 *
 * Revision 2.4  1998/05/19  15:28:41  kleiner
 * strtol
 *
 * Revision 2.3  1998/05/18  07:03:55  kleiner
 * *** empty log message ***
 *
 * Revision 2.2  1998/05/17  17:39:05  Administrator
 * *** empty log message ***
 *
 *  Revision 2.1  1998/05/15 07:13:32  kleiner
 *  *** empty log message ***
 *
 * Revision 1.2  1998/05/07  16:18:56  kleiner
 * *** empty log message ***
 *
 */

/*********************************************************************/
/*        FRAME WORK FOR PROPOSITIONAL LEARNING                      */
/*        Versao 1.0 (10/12/1996)       FILE: domain.c               */
/*  Developed by: Joao Gama, Luis Torgo                              */
/*                LIACC - Uni.do Porto                               */
/*                {jgama, ltorgo}@ncc.up.pt                          */
/*********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "framework.h"
#include "measures.h" /* US added */

#define digit(ch)      ((ch) >= '0' && (ch) <= '9')
#define letter(ch)     ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
#define _DATA_	       0
#define _STRUCTURE_    1
#define MAX_STR_SIZE   512  /*256*/

/* change koepf beginning */

int REGRESSION = FALSE;
int ONLY_NUMERIC = FALSE;

/* change koepf end */

typedef struct {
  int       type_cell;
  int       order;
  void      *data;
} struct_cell;

/* US no longer used
static List    *ValsStructAttr(char *, unsigned int *);
*/
static void    ShowStructVals(List *lst);
static int     NodeId(char *, List *);
static char    *GetAtom(int *, char **);
static List    *GetTerms(char **);
static List    *SInsert(void *,int , List *);
static int     ShowStruct(List *,int );
static int     DepthFirst(List *lst);
static List    *SearchNode(int node, List *tree);

int count_attributes (FILE *DomFile);
DomainInfo *ReadDomainFile(FILE *f, int nr_attr);
int ProcessAttribute(char *line, int *type, void **vals, int *nr_vals, int *isint) ;

/* =================================================================
   READS A FILE WITH THE DOMAIN ATTRIBUTES DECLARATIONS.
   Returns a structure that stores internally that information.
   ----------------------------------------------------------------- */

DomainInfo *_ReadDomain(char *Name, char *Ext)
{
  FILE            *f;
  DomainInfo      *Info = NULL;
  int             nr_attr;
  
  if ((f = fopen(new_strcat(Name, Ext),"r")) == NULL) {
    fprintf(stderr, "\nERROR: opening domain file %s\n", new_strcat(Name, Ext));
    fflush(stderr);
    fclose(f);
    exit(2);
  }

  if ((nr_attr = count_attributes (f)) == 0) {
    fprintf (stderr, 
	     "\nERROR: No attribute definitions in domain file '%s'\n",
	     new_strcat(Name, Ext));
    fclose(f);
    exit(3);
  }
  rewind(f);
  
  Info = ReadDomainFile(f, nr_attr);
  if (Info == NULL) {
    fprintf(stderr, "\nERROR: (ReadDomain) domain file corrupt  %s\n",
	    new_strcat(Name, Ext));
    fclose(f);
    exit(3);
  }
  
  fclose(f);
  
  return Info;
}

/* =================================================================
   READS A FILE WITH THE DOMAIN ATTRIBUTES DECLARATIONS.
   Returns a structure that stores internally that information.
   ----------------------------------------------------------------- */

DomainInfo *ReadDomainFile(FILE *f, int nr_attrs)
{
  fpos_t          old_pos;
  DomainInfo      *Info = NULL;
  AttrInfo        *ats = NULL;
  int             attr_nr;
  char            ch, *cut_pos;
  char            *buf = NULL;
  int             buf_size, str_pos;
  void            *vals;
  int             type, nr_vals;
  int             isint;
  

  if (f == NULL) return NULL;
  
  if (((Info = (DomainInfo *) malloc(sizeof(DomainInfo))) == NULL) || \
      (ats =(AttrInfo *) calloc(nr_attrs,sizeof(AttrInfo))) == NULL) {
    fprintf(stderr, "\nERROR: ReadDomain out of Memory\n");
    return NULL;
  }

  Info->nr_attrs = nr_attrs;
  Info->attrs = --ats;
 
  attr_nr = 0;
  while (!feof(f)) {
    while (contained((ch = fgetc(f)), " \t")) {};  /* skip leading whitespaces */
    if ((!contained(ch,"#|\n\0")) && (ch != EOF)) {  /*	regular Line */
      if (attr_nr >= nr_attrs) {
	fprintf (stderr, 
		 "\nERROR: reading attribute definitions in domain file \n");
	free(++ats);
	free(Info);
	return NULL;
      }
      ungetc(ch, f);
      fgetpos(f, &old_pos);
      buf_size = 1;
      while((!feof(f)) && (fgetc(f) != '\n')) buf_size++;
      fsetpos(f, &old_pos);
      free(buf);
      buf = (char *) calloc((size_t) buf_size, sizeof(char));
      buf = fgets(buf, buf_size, f);
      str_pos = buf_size - 2;
      while (contained(buf[str_pos], " \t")) str_pos--; /* skip trailing whitespaces */
      if ((buf[str_pos] == '.') && (buf[str_pos-1] != '\\')) {
	buf[str_pos] = '\0';
      }
      else {
	buf[str_pos+1] = '\0';
      }
      
/*       fprintf(stderr, "Zeile: %d\n  %s\n", attr_nr, buf); */
      
      cut_pos = buf;

      /*   fprintf(stderr, ">%s<\n", buf); */

      while (*cut_pos != '\0' && !contained(*cut_pos, " ,;:\t")) {

	/* zum Testen */
	/*   i++;  */
	/*   fprintf(stderr, "%d: %c  ", i, *cut_pos);*/
	/* zum Testen */

	cut_pos++;

      }

      /*  fprintf (stderr, "\n"); */

      if (attr_nr == 0) {       /* class attribute */
	
	/* change koepf beginning */

	/*	if (*cut_pos != ','){ */	
	if (!contained(*cut_pos, ",")){

	REGRESSION = TRUE;
	  Info->attrs[nr_attrs].name = new_strcpy("target variable");
	  cut_pos = buf;

	  Info->attrs[nr_attrs].isint = FALSE;
	  Info->attrs[nr_attrs].nr_unknown = 0;
	  Info->attrs[nr_attrs].nr_vals = nr_vals;
	  Info->attrs[nr_attrs].vals = vals;
	  Info->attrs[nr_attrs].type_attr = type;
	}
	else

	/* change koepf end */
	
	if (*cut_pos == ':') {  /* field name found */
	  *cut_pos = '\0';
	  Info->attrs[nr_attrs].name = new_strcpy(buf);
	  cut_pos = cut_pos + 1;
	}
	else {                  /* no field name found */
	  Info->attrs[nr_attrs].name = new_strcpy("class");
	  cut_pos = buf;
	}
	if(!ProcessAttribute(cut_pos, &type, &vals, &nr_vals, &isint)) {
	  fprintf(stderr, "ERROR: expecting values for class attribute\n %s\n", cut_pos);
	  free(++ats);
	  free(Info);
	  return NULL;
	}
	if ((type != nominal) || (nr_vals < 1)) {
	  fprintf(stderr, "ERROR: expecting values for class attribute\n %s\n", cut_pos);
	  free(++ats);
	  free(Info);
	  return NULL;
	}
	Info->attrs[nr_attrs].isint = FALSE;
	Info->attrs[nr_attrs].nr_unknown = 0;
	Info->attrs[nr_attrs].nr_vals = nr_vals;
	Info->attrs[nr_attrs].vals = vals;
	Info->attrs[nr_attrs].type_attr = type;
	}
      else {                  /* no class attribute */
	if (*cut_pos != ':') {  /* no field name found */
	  fprintf(stderr, "ERROR: expecting attribute name %d\n", attr_nr+1);
	  free(++ats);
	  free(Info);
	  return NULL;
	}
	*cut_pos = '\0';
	Info->attrs[attr_nr].name = new_strcpy(buf);
	cut_pos = cut_pos + 1;
	if(!ProcessAttribute(cut_pos, &type, &vals, &nr_vals, &isint)) {
	  fprintf(stderr, "ERROR: expecting values for attribute\n %s\n", cut_pos);
	  free(++ats);
	  free(Info);
	  return NULL;
	}
	Info->attrs[attr_nr].isint = isint;
	Info->attrs[attr_nr].nr_unknown = 0;
	Info->attrs[attr_nr].nr_vals = nr_vals;
	Info->attrs[attr_nr].vals = vals;
	Info->attrs[attr_nr].type_attr = type;
      }
      attr_nr++;
    }
    else {       /* skip comments */
      if (ch != '\n') {
	while ((!feof(f)) && ((ch = fgetc(f)) != '\n')) {};
      }
    }
  }   /* while(!feof(f)) */


  if (attr_nr != Info->nr_attrs) {
    fprintf (stderr, 
	     "\nERROR: reading attribute definitions in domain file\n");
    free(++ats);
    free(Info);
    return NULL;
  }

  free(buf);

  /* change koepf beginning */

  if (REGRESSION)
    Info->nr_attrs--;

  /* change koepf end */

  return Info; 

}

/* =================================================================
   READS A FILE WITH THE DOMAIN ATTRIBUTES DECLARATIONS.
   Returns the number of attributes.
   ----------------------------------------------------------------- 
   */


int 
ProcessAttribute(char *line, int *type, void **vals, int *nr_vals, int *isint) 
{
  char **s;
  int nr_words;
  
  *isint = FALSE;
  *nr_vals = 0;
  *vals = NULL;
  
  s = v_split(line, ",; \t",&nr_words);
  if (nr_words == 0) {
    fprintf(stderr, "ERROR: expecting value definition for attribute \n");
    return 0;
  }
  if (!strcmp(s[1],"continuous")) {
    *type = continuous;
    if (nr_words > 1) {
      fprintf(stderr, \
	      "WARNING: no further information after continuous expected\n %s\n", line);
    }
  }
  
  else if (!strcmp(s[1],"integer")) {
    *isint = TRUE;
    *type = continuous;
    if (nr_words > 1) {
      fprintf(stderr, \
	      "WARNING: no further information after integer expected\n %s\n", line);
    }
  }
  else {
    *type = nominal;
    *vals = s;
    *nr_vals = nr_words;
  }
  return TRUE;
}


/* =================================================================
   DISPLAYS THE INFORMATION ON A DOMAIN.
   ----------------------------------------------------------------- */
void ShowDomain(DomainInfo *D)
{
  /* (TK): unused: List *ptr; */
  int i,v;


  /*  ForAllAttributes(D,i) { */
  for(i = 1; i < D->nr_attrs; i++) {
    if (NomAttr(D,i)) {
      fprintf (stdout, "Attribute s.%d : %s\n",i,NameAttr(D,i));
      fprintf (stdout, "\tType : ");
      fprintf (stdout, "nominal\n");
    }
    else if (ContAttr(D,i)) {
      fprintf (stdout, "Attribute n.%d : %s\n",i,NameAttr(D,i));
      fprintf (stdout, "\tType : ");
      /*      fprintf (stdout, "continuous  isint: %d\n", D->attrs[i].isint); */
      if (D->attrs[i].isint) {
	fprintf (stdout, "continuous  isinteger: yes\n");
      }
      else {
	fprintf (stdout, "continuous  isinteger: no\n");
      }
    }
    else if (OrdAttr(D,i))
      fprintf (stdout, "ordered\n");
    else if (IntAttr(D,i))
      fprintf (stdout, "integer\n");
    else if (StructAttr(D,i))
      fprintf (stdout, "structured\n");
    if (NValsAttr(D,i) > 0) {
      fprintf (stdout, "\tNr.Different Values : %d\n",NValsAttr(D,i));
      if (StructAttr(D,i))
	ShowStructVals(SValsAttr(D,i));
      else {
	fprintf (stdout, "\tValues :: %s",LblValId(D,i,1));
	for(v=2;v<=NValsAttr(D,i);v++) fprintf (stdout, ", %s",LblValId(D,i,v));
	fprintf (stdout, "\n");
      } 
    }
    fprintf (stdout, "\n");
  }
  fflush (stdout);
}


void ShowTarget(DomainInfo *D)
{
  /* (TK): unused: List *ptr; */
  int i,v;

  i = D->nr_attrs;
  fprintf (stdout, "Target Attribute s.%d : %s\n",i,NameAttr(D,i));
  fprintf (stdout, "\tType : ");
  fprintf (stdout, "nominal\n");

  if (NValsAttr(D,i) > 0) {
    fprintf (stdout, "\tNr.Different Values : %d\n",NValsAttr(D,i));
    fprintf (stdout, "\tValues :: %s",LblValId(D,i,1));
    for(v=2;v<=NValsAttr(D,i);v++) fprintf (stdout, ", %s",LblValId(D,i,v));
    fprintf (stdout, "\n");
  } 
  fprintf (stdout, "\n");
  fflush (stdout);
}

/* =================================================================
   OBTAINS THE INTERNAL ID OF AN ATTRIBUTE VALUE.
   ----------------------------------------------------------------- */
unsigned int IdValLbl(DomainInfo *D, unsigned int IdAttr, char *Value)
{
  unsigned int pos=1;
  char **values;

  if (ContAttr(D,IdAttr)) return(0);
  if (StructAttr(D,IdAttr))
    return(NodeId(Value,SValsAttr(D,IdAttr)));

  values = (char **) D->attrs[IdAttr].vals;
  for(pos = 1; pos <= NValsAttr(D,IdAttr); pos++) 
    if (!strcmp(values[pos], Value)) return pos;
  return 0;
}
      
/* =================================================================
   OBTAINS THE EXTERNAL LABEL THAT CORRESPONDS TO AN INTERNAL 
   ATTRIBUTE ID.
   ----------------------------------------------------------------- */
char *LblValId(DomainInfo *D, unsigned int IdAttr, unsigned int IdVal)
{
  List *p;
  struct_cell *e;
  char **values;

  if (StructAttr(D,IdAttr)) {
    p = SearchNode(IdVal,SValsAttr(D,IdAttr));
    if (p) { 
      e = (struct_cell *) head(p); 
      return(e->data); 
    }
    return(NULL);
  } 
  values = (char **) D->attrs[IdAttr].vals;
  return(values[IdVal]);
}  


/* =================================================================

   ----------------------------------------------------------------- */
static char *GetAtom(int *status, char **h)
{
  char *str, temp[512]; /*256 (TK): unused: , ch; */
  int i=0;


  str = *h;
  while (*str == ' ') ++str;
  while(str) {
    if (digit(*str) || letter(*str) || *str == '_' || *str == '.')
      temp[i++] = *str++;
    else {
      temp[i++] = '\0';
      switch(*str) {
      case ',': 
      case ' ':
      case '\t':
	*status = i == 1 ? 0: 1;
	break;
      case '[': 
	*status = i == 1 ? 2: 3;
	break;
      case ']': 
	*status = i > 1 ? 4 : 5;
	break;
      default : 
	*status = -1;
      }
      ++str;
      *h = str;
/* fprintf (stdout, "temp = %s; status = %d",temp,*status); */
      return(new_strcpy(temp));
    }
  }
  /* (TK) */
  return NULL;
}


/* =================================================================

   ----------------------------------------------------------------- */
static List *GetTerms(char **h)
{
  char *atom;
  int status;
  
  atom = GetAtom(&status, h);

  switch (status) {
  case -1: 
    fprintf (stdout, "Invalid character reading structured attribute declaration\n");
    exit(0);
  case 0: return(GetTerms(h));
  case 1: return(SInsert(atom, 0, GetTerms(h)));
  case 2: return(SInsert(GetTerms(h), 1,GetTerms(h)));
  case 3: return(SInsert(atom, 0,SInsert(GetTerms(h), 1,GetTerms(h))));
  case 4: return(SInsert(atom, 0,NULL)); 
  case 5: return(NULL);
  }
  /* (TK): */
  return (NULL);
}


/* =================================================================

   ----------------------------------------------------------------- */
static List *SInsert(void *element,int type, List *lst)
{
  struct_cell *newptr = (struct_cell *) malloc(sizeof(struct_cell));
  if (! newptr)
    fprintf(stderr, "E)insert: Out of memory!\n");
  else {
    newptr->data = element;
    newptr->type_cell = type;
    return(insert(newptr, lst));
  }
  return(NULL);
}


/* =================================================================

   ----------------------------------------------------------------- */
static int DepthFirst(List *lst)
{
  int i=0;
  static int ord=1;
  struct_cell *element;

  while (lst) {
    element = (struct_cell *) head(lst);
    if (element->type_cell == _DATA_) {
      element->order=ord++;
      i++;
    } else i+=DepthFirst((List *) element->data);
    lst = tail(lst);
  }
  return(i);
}


/* =================================================================

   ----------------------------------------------------------------- */
static void ShowStructVals(List *lst)
{
  ShowStruct(lst,1);
}

/* (TK): added int */
static int ShowStruct(List *lst,int sp)
{
  struct_cell *element;
  int i;


  while(lst){
    element = (struct_cell *) head(lst);

    switch(element->type_cell){
    case _DATA_:
      for(i=1;i<=sp;i++) putchar(' ');
      fprintf (stdout, "(%s)#%d\n",(char *) element->data,element->order);
      break;
    case _STRUCTURE_:
      ShowStruct((List *)element->data,sp + 5);
    }
    lst = tail(lst);
  }
  /* (TK): */
  return 0;
}




/* =================================================================

   ----------------------------------------------------------------- */
static int NodeId(char *name, List *values)
{
  struct_cell *element;
  int i;

  if (!values) return(-1);
  else {
    element = (struct_cell *) head(values);
    if (element->type_cell == _DATA_) { 
      if (!strcmp(element->data,name)) return(element->order); 
      else return(NodeId(name,tail(values)));
    }
    else 
      /* (TK) added '!= 0' to avoid warning */
      if ((i=NodeId(name,(List *) element->data)) != 0) return i;
      else return(NodeId(name,tail(values)));
  }
}




/* =================================================================

   ----------------------------------------------------------------- */
static List *SearchNode(int node, List *tree)
{
  struct_cell *element, *ele2;
  List *p;

  if (!tree) return(NULL);
  else {
    element = (struct_cell *) head(tree);
    if (element->type_cell == _DATA_) { 
      if (element->order == node) return(tree); 
      else return(SearchNode(node,tail(tree)));
    } else {
      p = tail(tree);
      if (p) {
	ele2 = (struct_cell *) head(p);
	if (ele2->order <= node) return(SearchNode(node,p));
	else return(SearchNode(node,element->data));
      }
      else return(SearchNode(node,element->data));
    }
  }
}


/* =================================================================
   CHECKS IF A VALUE SUBSUMES ANOTHER IN A STRUCTURED ATTRIBUTE
   HIERARCHY.
   ----------------------------------------------------------------- */
int IsA(unsigned int Spec, unsigned int Gen, List *values)
{
  List *p;
  struct_cell *e;

  if (Spec < Gen) return 0;
  else if (Spec == Gen) return 1;
  else {
    p = SearchNode(Gen,values);
    if (p && tail(p)) {
      e = (struct_cell *) head(tail(p));
      if (e->type_cell == _STRUCTURE_) {
	if (tail(tail(p))) {
	  e = (struct_cell *) head(tail(tail(p)));
	  if (e->order > Spec) return 1;
	}
	else return 1;
      }
    }
    return 0;
  }
}



/* =================================================================
   OBTAINS THE INTERNAL ID CORRESPONDING TO AN ATTRIBUTE NAME.
   ----------------------------------------------------------------- */
/* unsigned int IdAttr(DomainInfo *D, char *name) */
/* { */
/*   int i; */

/*   ForAllAttributes(D,i)  */
/*     if (!strcmp(name,NameAttr(D,i))) return(i); */
/*   return 0; */
/* } */



/* =================================================================
   READS A FILE WITH THE DOMAIN ATTRIBUTES DECLARATIONS.
   Returns the number of attributes.
   ----------------------------------------------------------------- 
   */
int
count_attributes (FILE *DomFile) {
  int	count = 0;
  char	ch;
 
  if (DomFile == NULL) {
    return 0;
  }
  
  while (!feof(DomFile)) {
    while (contained((ch = fgetc(DomFile)), " \t")) {};  /* Skip Whitespaces */
    if ((!contained(ch,"#|\n")) && (ch != EOF)) {  /* regular Line */
      count++;
    }
    if (ch != '\n') {     /* go to end of line */
      while ((!feof(DomFile)) && ((ch = fgetc(DomFile)) != '\n')) {};
    }
  }

    return count;
}  /* count_attributes */


