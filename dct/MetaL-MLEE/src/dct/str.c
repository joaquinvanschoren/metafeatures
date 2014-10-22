/*
 * $Source /user1/grimmer/clem/src/achim/src/RCS/str.c,v $
 * $Author: johann $
 * $Date: 2002/06/30 18:02:16 $
 * $Revision: 1.1.1.1 $
 */
/*
 *  $Log: str.c,v $
 *  Revision 1.1.1.1  2002/06/30 18:02:16  johann
 *  Initial CVS version
 *
 *  Revision 4.2  1998/07/21 12:20:25  spuhl
 *  mehods to filter "\" added
 *
 * Revision 4.1  1998/07/17  11:53:48  kleiner
 * switching to revision 4.1
 *
 * Revision 3.2  1998/07/02  08:24:44  spuhl
 * *** empty log message ***
 *
 * Revision 3.1  1998/06/23  12:41:43  kleiner
 * *** empty log message ***
 *
 * Revision 2.4  1998/05/20  10:46:16  kleiner
 * *** empty log message ***
 *
 * Revision 2.3  1998/05/18  07:04:13  kleiner
 * *** empty log message ***
 *
 * Revision 2.2  1998/05/17  17:39:31  Administrator
 * *** empty log message ***
 *
 *  Revision 2.1  1998/05/15 07:13:49  kleiner
 *  *** empty log message ***
 *
 * Revision 1.2  1998/05/07  16:19:13  kleiner
 * *** empty log message ***
 *
 */

/*********************************************************************/
/*        FRAME WORK FOR PROPOSITIONAL LEARNING                      */
/*        Versao 1.0 (10/12/1996)              FILE:str.c            */
/*  Developed by: Joao Gama, Luis Torgo                              */
/*                LIACC - Uni.do Porto                               */
/*                {jgama, ltorgo}@ncc.up.pt                          */
/*********************************************************************/
/************************************************/
/*      Strings Functions    		str.c   */
/************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include "framework.h"

/************************************************/
/*	new_strcpy(str)				*/
/*	Return a pointer to a new location 	*/
/*		of str				*/
/************************************************/
char *new_strcpy(char *str)
{
  char *p, *n;
  
  n = (char *) calloc(strlen(str)+1, sizeof(char));
  
  p = n;
  while(*str) *n++ = *str++;
  *n = '\0';
  return(p);
}
/************************************************/
/*	new_strcat(str1, str2)			*/
/*	Return a pointer to a string that 	*/
/*		concatenates str1 with str2	*/
/************************************************/
char *new_strcat(char *str1, char *str2)
{
  char *p;
  char *str = (char *) calloc(strlen(str1)+strlen(str2)+1, sizeof(char));
  
  p = str;
  while(*str1) *str++ = *str1++;
  while(*str2) *str++ = *str2++;
  *str = '\0';
  return(p);
}
/************************************************/
/*	is_a_int(str)				*/
/*	Return 	1 if all characters in str 	*/
/*		are numbers, 0 Otherwise	*/
/************************************************/
int is_a_int(char *str)
{
  if ((*str == '+') || (*str == '-'))	++str;
  while(*str) {
    if ((*str < '0') || (*str > '9'))
      return(0);
    ++str;
  }
  return(1);
}
/************************************************/
/*	is_a_str(str)				*/
/*	Return 	1 if all characters in str 	*/
/*	are alphabetic or numbers, 0 Otherwise	*/
/************************************************/
int is_a_str(char *str)
{
  while(*str) {
    if (!isalnum(*str))
      return(0);
    ++str;
  }
  return(1);
}
/************************************************/
/*	is_a_real(str)				*/
/*	Return 	1 if str is a real number	*/
/*		0 Otherwise			*/
/************************************************/
int is_a_real(char *str)
{
  if((*str == '+') || (*str == '-'))		++str;
  while( *str && (*str >= '0') && (*str <= '9'))	++str;
  if (*str == '.')				++str;
  while((*str >= '0') && (*str <= '9'))		++str;
  if ((*str == 'e') || (*str == 'E'))             ++str;
  if((*str == '+') || (*str == '-'))		++str;
  while( *str && (*str >= '0') && (*str <= '9'))	++str;
  if (*str == '\0')				return(1);
  return(0);
}

/************************************************/
/*	sequence(str1, str2)			*/
/*	Return 	1 if str1 begin's with str2	*/
/*		0 Otherwise			*/
/************************************************/
int starts_by(char *str1, char *str2)
{
  if(*str2 == '\0')	return(1);
  if(*str1 != *str2)	return(0);
  return(starts_by(++str1, ++str2));
}


/************************************************/
/*	eq(str1, str2)				*/
/*	Return 	1 if the number represented by 	*/
/*		str1 is equal to the number	*/
/*		represented by str2		*/
/*	     	0 Otherwise			*/
/************************************************/
int eq_numstr(char *str1, char *str2)
{
  if (atof(str1) == atof(str2))	return(1);
  return(0);
}
/************************************************/
/*	gt(str1, str2)				*/
/*	Return 	1 if the number represented by 	*/
/*		str1 is grater than the number	*/ 
/*		represented by str2		*/
/*	     	0 Otherwise			*/
/************************************************/
int gt_numstr(char *str1, char *str2)
{
  if (atof(str1) > atof(str2))	return(1);
  return(0);
}
/************************************************/
/*	lt(str1, str2)				*/
/*	Return 	1 if the number represented by 	*/
/*		str1 is less than the number	*/ 
/*		represented by str2		*/
/*	     	0 Otherwise			*/
/************************************************/
int lt_numstr(char *str1, char *str2)
{
  if (atof(str1) < atof(str2))	return(1);
  return(0);
}


/************************************************/
/*	strgt(str1, str2)			*/
/*	Return 	1 if str1 is grater than str2	*/ 
/*	     	0 Otherwise			*/
/************************************************/
int strgt(char *str1, char *str2)
{
  int len1, len2;
  len1 = strlen(str1);
  len2 = strlen(str2);
  if (memcmp(str1, str2, len1 < len2 ? len1 : len2) > 0)
    return(1);
  return(0);
}
/************************************************/
/*	strlt(str1, str2)			*/
/*	Return 	1 if str1 is less than str2	*/ 
/*	     	0 Otherwise			*/
/************************************************/
int strlt(char *str1, char *str2)
{
  int len1, len2;
  len1 = strlen(str1);
  len2 = strlen(str2);
  if (memcmp(str1, str2, len1 < len2 ? len1 : len2) < 0)
    return(1);
  return(0);
}


/************************************************/
/*	new_nstrcat(int n, str1,..., strn)		*/
/*	Return a pointer to a string that 	*/
/*	    concatenates str1 ... strn	        */
/************************************************/
char *new_nstrcat(int n, ...)
{
  char *s, *str, *str1;
  int wlen = 0, wn = n;
  va_list p;
  
  va_start(p, n);
  while (n--)
    wlen += strlen(va_arg(p, char *));
  va_end(p);

  va_start(p, n);
  str = (char *) calloc(wlen+1, sizeof(char));
  s = str;
  while(wn--) {
    str1 = (char *) va_arg(p, char *);
    while(*str1) *str++ = *str1++;
  }
  *str = '\0';
  va_end(p);
  return(s);
}

/************************************************/
/*	a_str(int n, str1,..., strn)		*/
/*	Return a pointer to an array of strings */
/************************************************/
char **a_str(int n, ...)
{
  int i = 0;
  va_list p;
  char **s = (char **) calloc(n+1, sizeof(char *));

  va_start(p, n);
  while(n--) 
    s[i++] = (char *) va_arg(p, char *);
  s[i] = NULL;
  va_end(p);
  return s;
}


/* =================================================================
   Splitting a string into a list of strings. The split is made by 
   indicating a string of characters that are taken as separators.
   ================================================================= */
List *split(char *str,char *splits)
{
  char *cpy, *new_str, *start;
  int i,c;
  List *lst=NULL;
  
  while ((*str) && (contained(*str,splits))) str++;
  if (*str) {
    start=str;c=1;
    str++;
    while ((*str) && (!contained(*str,splits))) {
      c++;
      str++;
    }
    new_str=(char *) malloc ((c+1)*sizeof(char));
    cpy=new_str;
    for(i=0;i<c;i++) *cpy++ = *start++;
    *cpy='\0';
    lst=(List *) malloc(sizeof(List));
    lst->data=new_str;
    lst->next=split(str,splits);
  }
  return(lst);
}


/* =================================================================
   This function is similar to the previous one (split), but instead
   of obtaining a list of strings it obtains a vector of strings.
   It also returns the number of strings that were split.
   NOTE : the function you should actually use is v_split (without
   the dash) that is defined as a macro in str.h
   ================================================================= */
char **_v_split(char *str, char *splits, int *nwrds, int nr)
{
  char *word, **arr_str;

  if ((!(*str)) || ((word=ReadWord(&str,splits)) == NULL)) {
    if (nr > 1) {
      *nwrds = nr - 1;
      arr_str = (char **) malloc((nr - 1) * sizeof(char*));
      return --arr_str;
    } 
    else return NULL;
  }
  arr_str = _v_split(str,splits,nwrds,nr+1);
  arr_str[nr] = word;
  return arr_str;
}
  
/* =================================================================
   This function consumes a string from another until a separator is
   reached.
   ================================================================= */
/* (TK): used to be static */
char *ReadWord(char **str, char *sep)
{
  register int i = 0;
  char temp[512], ch; /* 256 */
  int special;
  
  special = FALSE;
  ch = **str;;
  while (*str && contained(ch, sep))      { (*str)++; ch = **str;}
  if (ch != '\0') {
    while (((!contained(ch,sep)) || special) && (ch)) {
      if (special) {
	temp[i++]=ch;
	(*str)++;
	ch = **str;
	special = FALSE;
      }
      else if (ch == '\\') {
	(*str)++;
	ch = **str;
	special = TRUE;
      }
      else {
	temp[i++]=ch;
	(*str)++;
	ch = **str;
      }
    }
    temp[i]='\0';
    return(new_strcpy(temp));
  }
  return NULL;
}

/* =================================================================
   Checking if a character is contained in a string.
   1 - yes; 0 - no
   ================================================================= */
int contained(char ch,char *p)
{
  while ((*p) && (*p != ch)) p++;
  if (*p) return(1);
  return(0);
}



