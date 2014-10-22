//-*-Mode:c++ -*-
//== FILE HEADER BEGIN
//
// = FILENAME
//   mygetopt.cc
//
// = AUTHOR(S)
//   Johann Petrak 
//    
// = COPYRIGHT
//   Copyright (1997) Austrian Research Institute
//   for Artificial Intelligence, Vienna, Austria
//
// =================================================

#include<stdio.h>

int  curindex = 1;
  
char mygetopt(int argc, char ** argv, char * str, char *& value)
{
  char optchar;
  char * curopt;
  
  // is there any argument at all (not counting the program name)?
  if (curindex >= argc) return '\0';
  
  // get next option
  curopt = argv[curindex++];

  // hmmm it is not an option 
  if (*curopt++ != '-') return '?';

  // set the option character and skip it
  optchar = *curopt++;

  // look if the option char is found in the string
  while (* str && *str != optchar) str++;

  // if we didint find it ...
  if (! *str) return '?';

  // otherwise look if this option takes an argument,
  // if yes, put it into optarg
  if ( *++str == ':')
  {
    // if there was no space between option character and the option argument
    if (*curopt) value = curopt;
    // otherwise we take the next command argument, if there is one
    else if (argc>curindex) value = argv[curindex++];
    else optchar = '?';
  }
  return optchar;

#if 0
  int ret = getopt(argc,argv,str);
  if (ret == EOF) ret = 0;
  value =  optarg;
  return (char)ret;
#endif
}

