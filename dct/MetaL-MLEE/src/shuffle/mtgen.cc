//-*-Mode:C++-*- 

//== FILE HEADER BEGIN
//
// = FILENAME
//   mtgen.cc
//
// = AUTHOR(S)
//   Johann Petrak 
//
// = DESCRIPTION
//   Generate a new random state file. 
//
// = SYNOPSIS
//   mtgen > statefile [-r seed | -R statefile] [-O statefile] [-a N] [-h]
//
// = VERSION
//   $Revision: 1.1.1.1 $
//
// = DATE
//   $Date: 2002/06/30 18:02:16 $
//
// = COPYRIGHT
//   Copyright (1997) Austrian Research Institute
//   for Artificial Intelligence, Vienna, Austria
//
//===============



#include<string>
#include<vector>
#include<algorithm>
#include<fstream.h>
// NOTE: might need <fstream> instead!
#include<stdio.h>
#include<mtcc.h>

char mygetopt(int argc, char ** argv, char * str, char *& value);

void usage(char * command)
{
  cerr << "Usage:\n";
  cerr << "  " << command 
       << "  > statefile [-r seed | -R statefile] [-a N] [-h]\n"
       << "  -r seed: Random seed, if =0 initialize from time\n"
       << "  -R statefile: file containing a MT random state\n"
       << "     if neither -r nor -R is supplied, -r 0 is assumed\n"
       << "  -a N: advance state by N calls to the generator (default 1)\n"
       << "  -O statefile: file to contain new MT random state\n"
       << "  -h show this help"; 
}


int main(int Argc, char** Argv)
{
  // Print programmname und version
  //const char * compile_date = __DATE__;
  //const char * compile_time = __TIME__;
  //cerr << Argv[0] << ": sample, $Revision: 1.1.1.1 $ built on "
  //     << compile_date << " at " << compile_time << endl;
    
  char *   statefile  = 0;
  char *   outstatefile  = 0;
  unsigned long seed = 0;
  unsigned long advance = 1;

    // ****************************
    // process command line options 
  char o;
  char * value;

  bool gotFlag = false;

  while ( (o = mygetopt(Argc, Argv,
		      "r:R:a:O:", value)) != '\0' )
  {

    switch (o)
    {
      case 'a':
      advance = atol(value);
      break;

      case 'r':
      if (gotFlag) {
	cerr << "Only one of -r and -R is allowed!\n";
	exit(1);
      }
      gotFlag = true;
      seed = atol(value);
      break;

      case 'R':
      if (gotFlag) {
	cerr << "Only one of -r and -R is allowed!\n";
	exit(1);
      }
      gotFlag = true;
      statefile = value;
      break;

      case 'O':
      outstatefile = value;
      break;

	
      case '?':
      cerr << "  Unknown option\n";
      usage(Argv[0]);
      exit(1);

      case 'h':
      usage(Argv[0]);
      exit(0);
    }
  }
 
  RandomMT rand;

  if (statefile)
    rand.read_state(statefile);
  else
    rand.initialize(seed);

  for(unsigned long i=0; i<advance; i++) 
    rand();

  cout << rand();
  if (outstatefile!=0)
    rand.write_state(outstatefile);
} 



















