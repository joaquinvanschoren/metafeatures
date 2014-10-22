/*
 * $Source /user1/grimmer/clem/src/achim/src/RCS/data_structures.c,v $
 * $Author: johann $
 * $Date: 2002/06/30 18:02:15 $
 * $Revision: 1.1.1.1 $
 */
/*
 *  $Log: data_structures.c,v $
 *  Revision 1.1.1.1  2002/06/30 18:02:15  johann
 *  Initial CVS version
 *
 *  Revision 4.1  1998/07/17 11:53:59  kleiner
 *  switching to revision 4.1
 *
 * Revision 3.2  1998/07/02  08:24:52  spuhl
 * *** empty log message ***
 *
 * Revision 3.1  1998/06/23  12:41:49  kleiner
 * *** empty log message ***
 *
 * Revision 2.3  1998/05/18  07:04:19  kleiner
 * *** empty log message ***
 *
 * Revision 2.2  1998/05/17  17:39:04  Administrator
 * *** empty log message ***
 *
 *  Revision 2.1  1998/05/15 07:13:55  kleiner
 *  *** empty log message ***
 *
 * Revision 1.2  1998/05/13  13:46:04  kleiner
 * version 2.2
 *
 */

/*********************************************************************/
/*        FRAME WORK FOR PROPOSITIONAL LEARNING                      */
/*        Versao 1.0 (10/12/1996) FILE:data_structures.c             */
/*  Developed by: Joao Gama, Luis Torgo                              */
/*                LIACC - Uni.do Porto                               */
/*                {jgama, ltorgo}@ncc.up.pt                          */
/*********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "framework.h"

static List *insert_sort(List *l1,List *l2);
List *quick_sort(List *lst,int (* func)(void *e1, void *e2));

/* ################################################################# */
/* LISTS                                                             */
/* ################################################################# */

/* =============================================================
   This function adds an element to the head of a list returning
   the resulting list.
   ============================================================= */
List *insert(void *element, List *lst)
{
  List *newptr = (List *) malloc(sizeof(List));
  if (! newptr)
    fprintf(stderr, "lists.c:insert: Out of memory!\n");
  else {
    newptr->data = element;
    newptr->next = lst;
  }
  return(newptr);
}
 

/* =============================================================
   This function adds an element to the end of a list returning
   the resulting list.
   ============================================================= */
List *insert_end(void *element, List *lst)
{
  List *newptr = (List *) malloc(sizeof(List));
  if (! newptr)
    fprintf(stderr, "lists.c:insert_end: Out of memory!\n");
  else {
    newptr->data = element;
    newptr->next = NULL;
    if (lst) tail(last(lst)) = newptr;
    else lst = newptr;
  }
  return(lst);
}
 
/* =============================================================
   Inserts an element in an ordered list returning the resulting
   ordered list. It uses the function "func" as the sorting
   function for determining the correct position of the element.
   ============================================================= */
List *insert_ord(void *element,List *lst,int (* func)(void *, void *))
{
  List *newptr   = (List *) malloc(sizeof(List));
  List *lastptr  = lst;
  List *firstptr = lst;
  
  if (!newptr) {
    fprintf(stderr,"lists.c:insertOrd: Out of memory !\n");
    return(NULL);
  }
  newptr->data = element;
  if (!lst || func(lst->data, newptr->data)) {
    newptr->next = firstptr;
    return(newptr);
  }
  else {
    while (lst && !func(lst->data, newptr->data)) {
      lastptr = lst;
      lst = lst->next;
    }
    newptr->next = lastptr->next;
    lastptr->next = newptr;
    return(firstptr);
  }
}




/* =============================================================
   Checks if an element belongs to a list. If yes then returns a
   pointer to the element in the list.
   The notion of belonging is implemented by a function "func"
   which is passed to this function. It's needed due to the fact
   that this works with general lists, thus we don't know at start
   what is an element being equal to other as we don't know what
   are elements.
   ============================================================= */
List *belongs(void *element, List *lst,int (*func)(void *, void *))
{
  while (lst && !func(element,head(lst))) lst = tail(lst);
  if (!lst) return(NULL);
  else return(lst);
}



/* =============================================================
   Returns a pointer to the Nth element of a list.
   ============================================================= */
List *nth(unsigned long n,List *lst)
{
  if (n < 1)	return(lst);
  while (lst) {
    if (n == 1)	break;
    lst = tail(lst);
    --n;
  }
  return(lst);
}



/* =============================================================
   It returns a pointer to the position in the list where an
   element is. It also returns the order position of that element
   in the list. It returns 0 if the element is not in the list. 
   It needs a function "func" to compare elments (see function 
   belongs above for a justification).
   ============================================================= */
List *position(void *element, List *lst, int (*func)(void *, void *),unsigned 
long *nr)
{
  unsigned long pos = 1;
  
  while (lst && !func(element,head(lst))) {
    lst = tail(lst);
    ++pos;
  }
  if (!lst)	*nr = 0;
  else		*nr = pos;
  return(lst);
}



/* =============================================================
   Removes an element from a list. Again a function for checking
   equality is needed.
   ============================================================= */
List *delete(void *element,List *lst,int (*func)(void *, void *))
{
  List *ptrfirst = lst, *ptrlast = lst;
  if (! lst)       return(NULL);
  if (func(head(lst), element))  return(tail(lst));
  while(lst && !func(element, head(lst))) {
    ptrlast = lst;
    lst = tail(lst);
  }
  if (lst) {
    ptrlast->next = tail(lst);
    free(lst);
  }
  return(ptrfirst);
}


/* =============================================================
   Removes the POS element of list LST returning the resulting list.
   It also returns a List pointer to the element that was removed.
   Example use :
      List *ele_rem;
      ....
      lst = remove_nth(5,lst,&ele_rem);
   ============================================================= */
List *remove_nth(unsigned long int pos, List *lst, List **removed)
{
  List *curr, *prev;

  if (pos == 1) {
    *removed = lst;
    return(tail(lst));
  } else {
    prev = lst;
    curr = tail(lst);
    pos--;
    while (pos > 1) { pos--; prev=curr; curr=tail(curr); }
    *removed=curr;
    tail(prev) = tail(curr);
    return(lst);
  }
}

/* =============================================================
   Concats the first argument list with the second argument list
   given the resulting list.
   ============================================================= */
List *lstcat(List *lst1, List *lst2)
{
  List *first = lst1;

  if (!lst1) return(lst2);
  while (tail(lst1)) lst1 = tail(lst1);
  lst1->next = lst2;
  return(first);
}


/* =============================================================
   This function changes the head of a list (i.e. changes the value
   of its data field).
   ============================================================= */
List *change_head(void *element, List *lst)
{
  if (!lst)	return(lst);
  free(lst->data);
  lst->data = element;
  return(lst);
}



/* =============================================================
   Counts the number of elements of a list.
   ============================================================= */
unsigned long count(List *lst)
{
  unsigned long int nr = 0;
  while(lst) {
    ++nr;
    lst = tail(lst);
  }
  return(nr);
}



/* =============================================================
   Gives the last element of a list
   ============================================================= */
List *last(List *lst)
{
  if (!lst) return(NULL);
  while (tail(lst)) lst = tail(lst);
  return(lst);
}



/* =============================================================
   "Cleans" a list by freeing up the memory used by its elements.
   NOTE : It does not remove the space occupied by the the data
   structures pointed by the "data" fields of the list.
   ============================================================= */
void clean(List *lst)
{
  List *p;

  while (lst) {
    p = lst;
    lst = tail(lst);
    free(p);
  }
}


/* =============================================================
   "Cleans" a list by freeing up the memory used by its elements.
   NOTE : It cleans also the elements pointed by the "data" fields.
   ============================================================= */
void clean_all(List *lst)
{
  List *p;

  while (lst) {
    p = lst;
    lst = tail(lst);
    free(p); free(p->data);
  }
}


/* =============================================================
   This function applies a given function to all elements of a
   list.
   ============================================================= */
void mapp(List *lst, void (* func) (void *))
{
  while (lst) {
    func(head(lst));
    lst = tail(lst);
  }
}


/* =============================================================
   This function does the same as the one above but does it in 
   reverse order (from the last element to the first).
   ============================================================= */
void mappInv(List *lst, void (* func) (void *))
{
  if (lst) {
    mappInv(tail(lst), func);
    func(head(lst));
  }
}



/* =============================================================
   Sorts a list using the quick sort algorithm. It needs as usual
   a function which determines the sort order.
   ============================================================= */
List *quick_sort(List *lst,int (* func)(void *, void *))
{
  List *ptr_small = NULL, *ptr_big = NULL;
  List *nextptr, *iniptr;
  
  fprintf (stderr, "#########  quick_sort !!!! \n");

  if (!lst)       return(NULL);
  iniptr = lst;
  lst = tail(lst);
  while(lst) {
    nextptr = tail(lst);
    if (func(head(iniptr), head(lst)))
      ptr_big = insert_sort(ptr_big, lst);
    else
      ptr_small = insert_sort(ptr_small, lst);
    lst = nextptr;
  }
  ptr_big = quick_sort(ptr_big, func);
  ptr_small = insert_sort(quick_sort(ptr_small, func), iniptr);
  return(lstcat(ptr_big,ptr_small));
}

static List *insert_sort(List *ptrFirst, List *ptrIns)
{
  ptrIns->next = ptrFirst;
  return(ptrIns);
}


/* =============================================================
   This function is primarly intended for debugging purpouses.
   It prints the elements of a list given a function "func" that
   knows how to print one element. The function stops at each end
   of page.
   ============================================================= */
void print_list(List *lst,void (* func) (void *))
{
  
  while (lst) {
    func(lst->data);
    lst = tail(lst);
  }
}
