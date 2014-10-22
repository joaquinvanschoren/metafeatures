/*
 * $Source /user1/grimmer/clem/src/achim/src/RCS/files.c,v $
 * $Author: johann $
 * $Date: 2002/06/30 18:02:16 $
 * $Revision: 1.1.1.1 $
 */
/*
 *  $Log: files.c,v $
 *  Revision 1.1.1.1  2002/06/30 18:02:16  johann
 *  Initial CVS version
 *
 *  Revision 3.4  1998/07/15 08:04:09  spuhl
 *  functions to read Domain in domain.c
 *  functions to read Data in instances.c
 *  files.c no longer used
 *
 * Revision 3.3  1998/07/13  06:43:42  spuhl
 * unused functions deleted
 *
 * Revision 3.2  1998/07/02  08:24:57  spuhl
 * *** empty log message ***
 *
 * Revision 3.1  1998/06/23  12:41:52  kleiner
 * *** empty log message ***
 *
 * Revision 2.5  1998/05/20  10:46:21  kleiner
 * *** empty log message ***
 *
 * Revision 2.4  1998/05/19  15:29:07  kleiner
 * strtol
 *
 * Revision 2.3  1998/05/18  07:04:21  kleiner
 * *** empty log message ***
 *
 * Revision 2.2  1998/05/17  17:39:12  Administrator
 * *** empty log message ***
 *
 *  Revision 2.1  1998/05/15 07:13:58  kleiner
 *  *** empty log message ***
 *
 * Revision 1.2  1998/05/14  16:48:10  kleiner
 * ..
 *
 */

/*********************************************************************/
/*        FRAME WORK FOR PROPOSITIONAL LEARNING                      */
/*        Versao 1.0 (10/12/1996)     FILE:files.c                   */
/*  Developed by: Joao Gama, Luis Torgo                              */
/*                LIACC - Uni.do Porto                               */
/*                {jgama, ltorgo}@ncc.up.pt                          */
/*********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "framework.h"


/* =================================================================

   ================================================================= */
char *ReadField(FILE *fi, char *sep)
{
  register int i = 0;
/*   char temp[_MAX_SIZE_LINE_]; */
  char *temp;
  char ch, lch, tch;
  char *field;
  fpos_t oldpos;
  
  ch = lch = *sep;
  while (contained(ch, sep))      ch = getc(fi);  /* skip Whitespaces */
  if (end_of_record(ch))          return(NULL);

  tch = ch;
  i = 0;
  if (fgetpos(fi, &oldpos)) {
    fprintf(stderr, "ReadField: Cant get FilePos\n");
    fflush(stderr);
    exit(455);
  }
  while(!(contained(tch, sep) || end_of_record(tch))) {
    i++;
    tch = getc(fi);
  }
  if ((temp = (char *) calloc(i+1, sizeof(char))) == NULL) {
    fprintf(stderr, "ReadField: Out of Memory\n");
    fflush(stderr);
    exit(456);
  }
  
  i = 0;
  if(fsetpos(fi, &oldpos)) {
    fprintf(stderr, "ReadField: Cant set FilePos\n");
    fflush(stderr);
    exit(457);
  }
  
  while(TRUE) {
   /*  if (i >= _MAX_SIZE_LINE_) { */
/*       fprintf (stderr, "ReadField: field to long (_MAX_SIZE_LINE_: %d)\n",  */
/* 	       _MAX_SIZE_LINE_); */
/*       temp[_MAX_SIZE_LINE_ - 1] = '\0'; */
/*       fprintf (stderr, "FOUND: \n%s\n", temp); */
/*       exit (7); */
/*     } */
 
    if (contained(ch, sep) || end_of_record(ch)) {
      if (!contained(lch, sep)) {
	if (temp[i-1] == '.') --i;
        temp[i++] = '\0';
        /* field = new_strcpy(temp); */
        if (end_of_record(ch))  ungetc(ch, fi);
        /* return(field); */
	return(temp);
      }
    }
    else
      temp[i++] = ch;
    lch = ch;
    ch = getc(fi);
  }
}

