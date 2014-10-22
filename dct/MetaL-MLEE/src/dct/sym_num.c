/*
 * $Source /user1/grimmer/clem/src/achim/src/RCS/sym_num.c,v $
 * $Author: johann $
 * $Date: 2002/06/30 18:02:16 $
 * $Revision: 1.1.1.1 $
 */
/*
 *  $Log: sym_num.c,v $
 *  Revision 1.1.1.1  2002/06/30 18:02:16  johann
 *  Initial CVS version
 *
 *  Revision 4.1  1998/07/17 11:53:50  kleiner
 *  switching to revision 4.1
 *
 * Revision 3.2  1998/07/02  08:24:45  spuhl
 * *** empty log message ***
 *
 * Revision 3.1  1998/06/23  12:41:44  kleiner
 * *** empty log message ***
 *
 * Revision 2.7  1998/06/16  16:28:41  kleiner
 * !!!!!!!!!!!!!!!!!!
 * added type setting for missing values (get_instances)
 *
 * Revision 2.6  1998/06/06  17:07:52  thor
 *  removed counting of missing values
 * is now done by instances.c
 *
 *  Revision 2.5  1998/06/04 16:15:55  kleiner
 *  'isint'
 *
 * Revision 2.4  1998/05/18  07:04:14  kleiner
 * *** empty log message ***
 *
 * Revision 2.3  1998/05/17  17:46:59  Administrator
 * *** empty log message ***
 *
 *  Revision 2.2  1998/05/17 17:39:32  Administrator
 *  *** empty log message ***
 *
 *  Revision 2.1  1998/05/15 07:13:50  kleiner
 *  *** empty log message ***
 *
 * Revision 1.3  1998/05/14  16:40:55  kleiner
 * *** empty log message ***
 *
 * Revision 1.2  1998/05/07  16:19:14  kleiner
 * *** empty log message ***
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "framework.h"
#include "measures.h"
#include "set_var.h"

/**********************************************************************
The following method makes the distinction between the structur DomainInfo
for numeric and for symbolic attributes

Input:
domain: structur for the whole dataset
nr_attr: nr. of attributes
Output:
two structures of type DomainInfor, one for numeric and one for symbolic
attributes
**********************************************************************/ 

DomainInfo *make_domain(DomainInfo *domain, int nr_attr) {

  int i, k = 1, v = 1;
  static DomainInfo result[2];  /* (TK): changed to static */
  DomainInfo *domain_num, *domain_sym;
  
  domain_num = (DomainInfo *) malloc (sizeof(DomainInfo));
  domain_sym = (DomainInfo *) malloc (sizeof(DomainInfo));
  domain_num -> attrs = (AttrInfo *) calloc ((domain -> nr_attrs)+1, sizeof(AttrInfo));
  domain_sym -> attrs = (AttrInfo *) calloc ((domain -> nr_attrs)+1, sizeof(AttrInfo));
  
  for(i = 1; i <= nr_attr; i++) {
    switch(TypeAttr(domain,i)) {
    case integer: 
    case continuous: 
      domain_num -> attrs[k].name = domain -> attrs[i].name; 
      domain_num -> attrs[k].type_attr = domain -> attrs[i].type_attr;  
      domain_num -> attrs[k].isint = domain -> attrs[i].isint;  /* TK */
      domain_num -> attrs[k].nr_unknown = domain -> attrs[i].nr_unknown; /* TK */
      domain_num -> attrs[k].nr_vals= domain -> attrs[i].nr_vals; 
      domain_num -> attrs[k].global_num=i;		/*	Wojciech Kwedlo	*/
      domain_num -> attrs[k++].vals = domain -> attrs[i].vals; 
      break;			
    case nominal   :
    case ordered   : 
      domain_sym -> attrs[v].name = domain -> attrs[i].name; 
      domain_sym -> attrs[v].type_attr = domain -> attrs[i].type_attr;  
      domain_sym -> attrs[v].isint = domain -> attrs[i].isint;   /* TK */
      domain_sym -> attrs[v].nr_unknown = domain -> attrs[i].nr_unknown; /* TK */
      domain_sym -> attrs[v].nr_vals = domain -> attrs[i].nr_vals; 
      domain_sym -> attrs[v].global_num=i;		/*	Wojciech Kwedlo	*/
      domain_sym -> attrs[v++].vals = domain -> attrs[i].vals; 
      break;				
    default:	/* (TK): added to avoid warning */
      break;
    }
  }  
  
  domain_num -> attrs[k].name = domain -> attrs[nr_attr].name;
  domain_num -> attrs[k].type_attr = domain -> attrs[nr_attr].type_attr;  
  domain_num -> attrs[k].isint = domain -> attrs[nr_attr].isint;   /* TK */
  domain_num -> attrs[k].nr_unknown = domain -> attrs[nr_attr].nr_unknown;   /* TK */
  domain_num -> attrs[k].nr_vals = domain -> attrs[nr_attr].nr_vals; 
  domain_num -> attrs[k].vals = domain -> attrs[nr_attr].vals; 
  
  domain_num -> nr_attrs = k;
  domain_sym -> nr_attrs = v-1;  
  
  domain_num -> root_name = domain -> root_name;
  domain_sym -> root_name = domain -> root_name;
  
  result[0] = *domain_num;
  result[1] = *domain_sym;
  
  return result;
  
}

/**************************************************************************
The following method makes the distinction of instances for numeric 
and symbolic attributes

Input:
ds: structur which contains the whole dataset
nr_num: nr. of numeric attributes
nr_sym: nr. of symbolic attributes
Output:
two structures of type AttrVal, one for symbolic and one for numeric 
attributes
**************************************************************************/

AttrVal ***
get_instances(Dataset *ds, int nr_num, int nr_sym) {

  int i,j;
  int k = 1,l = 1, nr_attr;
  unsigned int nr_ex;
  AttrVal ***result;
  AttrVal **inst_num, **inst_sym;
  
  
  result = (AttrVal ***) calloc (2, sizeof(AttrVal **));
  nr_ex = ds -> nr_exs; 
  nr_attr = nr_num + nr_sym -1;
 
  inst_num = (AttrVal **) calloc(nr_ex+1, sizeof(AttrVal *));
  inst_sym = (AttrVal **) calloc(nr_ex+1, sizeof(AttrVal *));
 
  inst_num[0] = (AttrVal *) calloc ((nr_ex+1) * (nr_num+1), sizeof(AttrVal));
  inst_sym[0] = (AttrVal *) calloc ((nr_ex+1) * (nr_sym+1), sizeof(AttrVal));
 
  for(i = 1; i <= nr_ex; i++)
    inst_num[i] = inst_num[i-1] + nr_num+1;

  for(i = 1; i <= nr_ex; i++)
    inst_sym[i] = inst_sym[i-1] + nr_sym+1;

  for(i = 1; i <= nr_attr; i++) {
   switch(TypeAttr(ds->domain, i)) {
    case integer:    
      for(j = 1; j <= nr_ex; j++) {
	if(NormalVal(ds -> instances[j][i])) {  
	  DValAttEx(inst_num[j],k) = DValAttEx(ds->instances[j],i);
	  TypeOfVal(inst_num[j][k]) = TypeOfVal(ds->instances[j][i]); 
	}
	else { /* TK */
	  TypeOfVal(inst_num[j][k]) = TypeOfVal(ds->instances[j][i]); 
	}
	/* else nr_unknown++;*/
      }  
      k++;           
      break;	
    case continuous: 
      for(j = 1; j <= nr_ex; j++) {
	if(NormalVal(ds -> instances[j][i])) {     
	  CValAttEx(inst_num[j],k) = CValAttEx(ds->instances[j],i);
	  TypeOfVal(inst_num[j][k]) = TypeOfVal(ds->instances[j][i]); 
	}
	else { /* TK */
	  TypeOfVal(inst_num[j][k]) = TypeOfVal(ds->instances[j][i]); 
	}
	/*else nr_unknown++;*/
      }       		         
      k++;  	
      break;
    case ordered:    
    case nominal:    
      for(j = 1; j <= nr_ex; j++) {
	DValAttEx(inst_sym[j],l) = DValAttEx(ds->instances[j],i);
	TypeOfVal(inst_sym[j][l]) = TypeOfVal(ds->instances[j][i]); 
      }  
      l++;   
      break;
    default:	/* (TK): added to avoid warning */
      break;
    }
  }  
  
  for(j = 1; j <= nr_ex; j++) {
    DValAttEx(inst_num[j],k) = DValAttEx(ds->instances[j],nr_attr);
    TypeOfVal(inst_num[j][k]) = TypeOfVal(ds->instances[j][nr_attr]); 
  }

  result[0] = inst_num;
  result[1] = inst_sym;

  return result;

} /* get_instances */

/**************************************************************************/


