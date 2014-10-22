//-*-Mode:C++-*- 

//== FILE HEADER BEGIN
//
// = FILENAME
//   shuffle.cc
//
// = AUTHOR(S)
//   Johann Petrak
//
// = DESCRIPTION
//   Shuffle a file of ASCII lines (terminated by \n)
//   The the actual permutation is determined by either
//   the random seed given or the random state file specified.
//   The program will optionally create or overwrite a random
//   state file after shuffling so that subsequent invocations
//   can produce new reproducable pseudo random shufflings.
//   The shuffled file is written to standard output.
//   When 0 is specified as a seed, the random number generator
//   is seeded by the current time
//
// = SYNOPSIS
//   shuffle -h
//
// = COPYRIGHT
//   Copyright (1997) Austrian Research Institute
//   for Artificial Intelligence, Vienna, Austria
// 
// = CHANGES
//   2000/10/25 1.3.2: was trying the alternate priority code
//      (option -A), but the code already there doesnt work
//      resolution postponed to later version, now -A is
//      just disabled.
//   1999/11/11 1.3.1: added output of compilation time to -v/-V
//      To work, this needs __COMPILEDON__ to be defined from outside!
//      The makefile has been changed accordingly.
//===============


static char * rcsid =
"@(#)$Id: shuffle.cc,v 1.1.1.1.2.1 2002/12/02 23:45:51 johann Exp $";

static char * pgmid =
"@(#)$Pgm: shuffle.cc compiled on " __DATE__ " at " __TIME__ " $";


#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<strings.h>
#include<errno.h>

#include<vector>
#include<fstream.h>

// the following is needed for matrix.h to work with EGCS C++
#define _NO_EXCEPTION

#include"matrix.h"
#include<mtcc.h>

// some systems do have this in stdio.h, some dont. 
// Uncomment, if you get duplicate/previous declaration errors
extern "C" const char *const  sys_errlist[]; 

char mygetopt(int argc, char ** argv, char * str, char *& value);

bool verbose = false;
bool veryverbose = false;

void usage(char * command)
{
  cerr << "Usage:\n";
  cerr << "  " << command 
       << "  < infile > outfile [-r seed | -R statefile] [-O statefile]\n"
       << "             [-s] [-b N] [-p f | -x N] [-t file] [-e file] [-v]"
#ifdef DEBUG
       << " [-V]"
#endif
       << "[-h]\n"
       << "  Read an ASCII file from standard input, optionally perform\n"
       << "  random shuffling, stratification for holdout or cross validation\n"
       << "  and output the full rearranged file or the training set/test set\n"
       << "  pair, to standard output or to files specified by options -t/-e.\n"
       << "  -r seed: Random seed, if =0 initialize from time\n"
       << "  -R statefile: file containing a MT random state\n"
       << "     if neither -r nor -R is supplied, no shuffling is done\n"
       << "  -O statefile: file to contain new random state\n"
       << "     this can be the same as for -R\n"

       << "  -p f: specify two blocks, specify fraction of training records\n"
       << "     between 0 and 1 or absolute number of records\n"
       << "  -x N: specify N (>2) blocks for cross validation\n"
       << "  -s: do stratification (assume C4.5 format of class labels)\n"
       << "     only reasonable together with -p or -x\n"
       << "  -b N: output Nth training/test block\n"
       << "     ignored if neither -p nor -x is given. If omited but\n"
       << "     -p or -x given, N is assumed to be the last possible block\n"

       << "  -t file: write training or full portion to file\n"
       << "  -e file: write testing/evaluation portion to file\n"
       << "     (ignored if neither -p nor -x is given)\n"
       << "     If -p/-x is given, but neither -e nor -t, the trainset will\n"
       << "     be written to stdout and the testset suppressed\n"

       << "  -v: print some statistics and process messages to stderr\n"
#ifdef DEBUG
       << "  -V: print debug info\n"
#endif
       << "  -h show this help\n"
       << "\n" << rcsid << "\n" << pgmid << "\n";
    ; 
}


typedef unsigned int SizeType;

int main(int Argc, char** Argv)
{
  // Print programmname und version
  //const char * compile_date = __DATE__;
  //const char * compile_time = __TIME__;
  //cerr << Argv[0] << ": shuffle, $Revision: 1.1.1.1.2.1 $ built on "
  //     << compile_date << " at " << compile_time << endl;
  
  char *   statefile  = 0;     // name of state file to read 
  char *   outstatefile  = 0;  // name of state file to write
  char *   outtrainfile = 0;   // name of train file to write
  char *   outtestfile = 0;    // name of test file to write


  unsigned long seed = 0;

  int blocks = 1;   // the number of folds/blocks to process

  bool doStrat = false;  // do we need to do stratification?

  int testblock = -1;    // number of block/fold for testset, starting with 0

  float percent = 0.0;   // percentage of number of records for holdout

  bool alternate = false;
  
  // ****************************
  // process command line options 
  char o;
  char * value;
  
  bool doShuffle = false;  // any of seed/state file specified?
  
  while ( (o = mygetopt(Argc, Argv,
			"r:R:O:st:e:b:p:x:Vvh", value)) != '\0' )
  {
    
    switch (o)
    {
      case 'p':
      sscanf(value,"%g",&percent);
      if (percent < 0.0 || (percent >= 1.0 && percent != int(percent)) ) {
	cerr << "Invalide percentage for training set size\n";
	exit(1);
      }
      break; 

      case 'b':
      testblock = atoi(value);
      if (testblock < 0) {
	cerr << "Invalid value for -b\n";
	exit(1);
      }
      break; 

      case 't':
      outtrainfile = value;
      break; 

      case 'e':
      outtestfile = value;
      break; 

      case 's':
      doStrat = true;
      break; 

      case 'A':
      alternate = true;
      break; 

      case 'x':
      blocks = atoi(value);
      if (blocks < 2) {
	cerr << "Number of blocks for cross validation no at least 2\n";
	exit(1);
      }
      break; 

      case 'r': 
      if (doShuffle) {
 	cerr << "Only one of -r and -R is allowed!\n";
	exit(1);
      }
      doShuffle = true;
      seed = atol(value);
      break;

      case 'R':
      if (doShuffle) {
	cerr << "Only one of -r and -R is allowed!\n";
	exit(1);
      }
      doShuffle = true;
      statefile = value;
      break;

      case 'O':
      outstatefile = value;
      break;

      case 'v':
      cerr << "Program version: " << rcsid << endl;
      cerr << "Compiled: " << __COMPILEDON__ << endl;
      verbose = true;
      break;
      
#ifdef DEBUG
      case 'V':
      cerr << "Program version: " << rcsid << endl;
      cerr << "Compiled: " << __COMPILEDON__ << endl;
      verbose = true;
      veryverbose = true;
      break;
#endif

      case '?':
      cerr << "  Unknown option\n";
      usage(Argv[0]);
      exit(1);

      case 'h':
      usage(Argv[0]);
      exit(0);

    }
  }
  
  // percentage / number of records only makes sense for holdout,
  // number of blocks must then be 2
  if (blocks > 2 && percent != 0.0) {
    cerr << "Blocks must be 2 (or omitted) if percentage specified\n";
    exit(1);
  }

  // if blocks were not specified, but percentage was, set blocks to
  // 2 to indicate holdout
  if (percent !=0 ) blocks = 2;

  // check blocknumber for testset must be 0 .. (blocks-1)
  if (testblock >= blocks) {
    cerr << "Test set number must be smaller than number of blocks [0..n-1]\n";
    exit(1);
  }

  // writing a statefile doesnt make sense if no random shuffling 
  // is done
  if (!doShuffle && (statefile != 0)) {
    cerr << "Cannot write a random state file without -r or -R option\n";
    exit(1);
  }

  // stratification is only reasonable for holdout/xval 
  if (doStrat && (percent == 0.0) && (blocks == 1)) {
    cerr << "Stratification only reasonable with -x or -p\n";
    exit(1);
  }

  // if no testblock number was givem assume last block
  if (testblock < 0 && blocks > 1) testblock = blocks - 1;

  // the maximum length of a single data record is BUFSIZE!
  const unsigned int BUFSIZE = 100000;
  char buf[BUFSIZE];

  SizeType i,j;
  int ret;
  char * tmp;

  // pointer to a dynamic array of pointers to where each record is stored
  char ** records;  

  // we increment the size of the above array by this amount each time
  // it gets too small
  const SizeType increment = 4096; 
  
  // the total size of the array so far
  SizeType size = increment;

  // Here we count the total amount of memory we need during the program
  SizeType total_mem = 0;

  // allocate the initial chunk of the array
  if (0 == (records = (char **) malloc(size * sizeof(char *)))) {
    cerr << "Could not allocate " << (size * sizeof(char *)) << " bytes\n";
    exit(1);
  }
  total_mem += (size * sizeof(char *));

  // the number of records read (so far)
  SizeType n_records = 0;

  // the length of the current record
  SizeType rec_length;

  ofstream os;

  RandomMT rand;


  void outRecords(ostream & out, 
		  char ** records, 
		  unsigned int from, 
		  unsigned int to);
  
  if (verbose) cerr << "Reading data ... ";
  while ( cin.getline(buf,BUFSIZE) ) {
    if (cin.gcount() == BUFSIZE) {
      cerr << "A single line must not be longer than "
	   << (BUFSIZE-2) << " bytes\n";
      cerr << "(record " << (n_records+1) << ")\n";
      exit(1);
    }
    if (n_records >= size) {
      size += increment;
      if (0 == (records = (char **) realloc(records, size * sizeof(char *)))) {
	cerr << "Could not allocate " << (size * sizeof(char *)) << " bytes\n";
	exit(1);
      }
      total_mem += (increment * sizeof(char *));
    }
    rec_length = strlen(buf) + 2; // add in newline and \0 characters
    if (0 == (records[n_records] = (char *)malloc(rec_length))) {
      cerr << "Could not allocate " << rec_length << " bytes\n";
      exit(1);
    }
    total_mem += rec_length;
#ifdef DEBUG
    if (records[n_records]==0) {
      cerr << "ASSERT ERROR " << __LINE__ 
	   << ": records[" << n_records << "] is 0!\n";
      exit(2);
    }
#endif
    memcpy(records[n_records], buf, rec_length);
    n_records++;
  }
  if (verbose) cerr << n_records << " records\n";
  
  // if n_records == 0, we take this as indication, that we just
  // want to create a state file
  if (n_records == 0) {
    if (outstatefile!=0) {
      if (verbose) cerr << "Inititalizing random generator: seed " << seed << endl;
      rand.initialize(seed);
      if (verbose) cerr << "Writing random state file\n";
      rand.write_state(outstatefile);
      if (verbose) cerr << "No records found, terminating (no error)\n";
      exit(0);
    } else {
      cerr << "No records and no random state file given!\n";
      exit(1);
    }
  }
    
  if (n_records < blocks) {
    cerr << "Too few data records: " << n_records 
	 << " need at least " << blocks << endl;
    exit(1);
  }

#ifdef DEBUG
  for (i = 0; i < n_records; i++) {
    if (records[i]==0) {
      cerr << "ASSET ERROR " << __LINE__ << ": records["<<i<<"] is 0!\n";
      exit(2);
    }
  }
#endif  

  if (doShuffle) {

    
    if (statefile) {
      ret = rand.read_state(statefile);
      if (ret != 0) {
	cerr << "Error " << ret
	     << " reading random state file " << statefile << endl;
	cerr << sys_errlist[ret] << endl;;
	exit(1);
      }
      if (verbose) cerr << "Random state file " << statefile << " read\n";
    } else
      rand.initialize(seed);

    // in order to shuffle the records, go through all records
    // starting with the first, and exchange each one with
    // one of the other records 
    // NOTE: this is done slightly differently in xval-prep.c
    // but the version there will not generate all partitions
    // with equal frequency (some will never be generated)
    unsigned long rand_i;
    if (verbose) cerr << "Shuffling records ... ";
    for (i = 0; i < n_records; i++) {
      rand_i = rand(n_records);
      assert(rand_i < n_records);
      tmp = records[i];
      records[i] = records[rand_i];
      records[rand_i] = tmp;
    }
    if (verbose) cerr << "done\n";
  }

#ifdef DEBUG
  for (i = 0; i < n_records; i++) {
    if (records[i]==0) {
      cerr << "ASSET ERROR " << __LINE__ << ": records["<<i<<"] is 0!\n";
      exit(2);
    }
  }
#endif  


  // if necessary do stratification
  char * pLabel(char *);

  if (blocks > 1) {

    if (percent >= n_records) {
      if (verbose) {
	cerr << "Warning: number of records specified (" << percent
	     << ") not smaller than number available (" << n_records
	     << ") setting to smaller value\n";
      }
      percent = n_records;
    }
    if (verbose) {
      if (percent > 0.0 && percent < 1.0)
	cerr << "Splitting data, fraction is " << percent << endl;
      else if (percent > 0.0) {
	cerr << "Splitting data, number of records is " << percent << endl;
      } else
	cerr << "Splitting data into " << blocks << "parts\n";
    }

    unsigned int fold;
    vector<unsigned int> foldsizes(blocks); // total number of records per fold
    vector<SizeType> location_fold(blocks); // index to first record of each fold

    if (doStrat) {
      
      if (verbose) cerr << "Doing stratification ...\n";
      
      // first we have to determine the number of labels
      vector<char *> lookup; // the lookup table for the label strings
      vector<SizeType> label_counts; // number or records with that label
      vector<SizeType> labels(n_records);  // what label index has each record?
      char * cur_labelp;
      SizeType n_labels;
      
      for (i = 0; i < n_records; i++) {
	j=0;
	cur_labelp = pLabel(records[i]); // get pointer to the label
	
	// try to find in lookup table
	while (j<lookup.size() && (0!=strcmp(cur_labelp,lookup[j])))
	  j++;
	//if not found ...
	if (j==lookup.size()) {
	  // add a copy of the string to the lookup table
	  lookup.push_back(strdup(cur_labelp));
	  // also add a counter for that label
	  label_counts.push_back(1);
	} else { // if we found the label in the lookup table ...
	  label_counts[j] += 1; // count
	}
	labels[i] = j;
      } // end for i
      n_labels = label_counts.size();
      
      if (verbose) {
	cerr << "Found " << n_labels << " different class labels\n";
	for (i=0; i<n_labels; i++) 
	  cerr << "  " << lookup[i] << ": " << label_counts[i] << endl;
      }
      // now we calculate how many rows we want in each block/fold for
      // each label
      matrix<unsigned int> rows_per_fold_label(blocks,n_labels);
      vector<unsigned int> fold_label(n_labels);
      
      unsigned int label;
      vector<unsigned int> remainders(n_labels);
      
      // if a percentage has been specified we need to do this a bit
      // differently ...
      long int sum = 0;
      if (percent > 0.0) {
	if (percent >= 1.0) {
	  // A specific number of records was specified 
	  // That count is stored in variable 'percentage'
	  for (label=0; label<n_labels; label++) {
	    i = label_counts[label];
	    rows_per_fold_label(0,label) 
	      = i * percent / n_records; 
	    // handle special case where this amounts to zero:
	    // there must be at least one record (otherwise the label
	    // wouldnt show up) so we assign 1 instead of zero.
	    // The rest is assigned to the other fold below ...
	    if (rows_per_fold_label(0,label) == 0)
	      rows_per_fold_label(0,label) = 1;
	    sum += rows_per_fold_label(0,label);
	    rows_per_fold_label(1,label) = i - rows_per_fold_label(0,label);
#ifdef DEBUG
	    if (veryverbose) {
	      cerr << "Label " << label << " count is " << label_counts[label]
		   << endl
		   << " Rows/fold0: " << rows_per_fold_label(0,label) << endl
		   << " Rows/fold1: " << rows_per_fold_label(1,label) << endl;
	    }
#endif
	  }
	  // since we truncated in our formula above we can be off
	  // the correct sum up to n_labels ... we will have to add
	  // the missing number of records (and subtract from testset sizes)
	  // BUT ONLY if there is at least one record in the testset!
#ifdef DEBUG
	  if (veryverbose) {
	    cerr << "Correcting counts\n";
	  }
#endif 
	  for (label=0; label<(percent - sum); label++) {
	    if (rows_per_fold_label(1,label) > 0) {
	      rows_per_fold_label(0,label) += 1;
	      rows_per_fold_label(1,label) -= 1;
#ifdef DEBUG
	      if (veryverbose)
		cerr << "Moving 1 record from fold 1 to 0 for label " 
		     << label << endl;
#endif
	    }
	  }
#ifdef DEBUG
	  if (veryverbose) 
	    for (label=0; label<n_labels; label++) {
	      if (verbose)
		cerr << "Class " << label << " fold 0: " 
		     <<  rows_per_fold_label(0,label) << " fold 1: "
		     <<  rows_per_fold_label(1,label) << endl;
	    }
#endif
	} else {
	  for (label=0; label<n_labels; label++) {
	    i = label_counts[label];
	    rows_per_fold_label(0,label) 
	      = (percent >= 1.0) ? percent : (i * percent);
	    // handle special case where this amounts to zero:
	    // there must be at least one record (otherwise the label
	    // wouldnt show up) so we assign 1 instead of zero.
	    // The rest is assigned to the other fold below ...
	    if (rows_per_fold_label(0,label) == 0)
	      rows_per_fold_label(0,label) = 1;
	    rows_per_fold_label(1,label) = i - rows_per_fold_label(0,label);
#ifdef DEBUG
	    if (veryverbose) {
	      cerr << "Label " << label << " count is " << label_counts[label]
		   << endl
		   << " Rows/fold0: " << rows_per_fold_label(0,label) << endl
		   << " Rows/fold1: " << rows_per_fold_label(1,label) << endl;
	    }
#endif
	  }
	}
      } else {
	// assign apropiate portion of row counts, remember remainders
	for(label = 0; label < n_labels; label++)
	  for(fold=0; fold<blocks; fold++) {
	    i = label_counts[label];
	    rows_per_fold_label(fold,label) = i / blocks;
	    remainders[label] = i % blocks;
	  }
	
	/// !!!! CAUTION: the following if will always execute
	/// the then part, since we have disabled the -A 
	/// option for now, since the code in the else part
	/// obviously doesnt work yet ...
	if (alternate == false) {
	// assign remainders: this method assures that all class
	// distributions are as euqal as possible, but some folds
	// may differ in size by more than 1
	for(label = 0; label < n_labels; label++)
	  for(fold=0; fold<remainders[label]; fold++) {
	    rows_per_fold_label(fold,label) += 1;
#ifdef DEBUG
	    if (veryverbose) {
	      cerr << "Label " << label << " count is " << label_counts[label]
		   << endl
		   << " Rows/fold " << fold << ": " 
		   << rows_per_fold_label(fold,label) << endl;
	    }
#endif
	  }
        } else {
	// this method of assigning remainders will assure that all
	// folds have as equal sizes as possible, but the class distributions
	// in each fold can vary slightly (maximum deviation in the order of
	// number of blocks)
	
	// assign remainders circularly
	fold = 0;
	for(label = 0; label < n_labels; label++)
	  for (i=0; i<remainders[label]; i++) {
	    rows_per_fold_label(fold,label) += 1;
	    fold++; if (fold == blocks) fold = 0; // wrap-increment fold
	  }
      }
      }
      // calculate fold sizes
      for(fold=0; fold<blocks; fold++)
      {
	foldsizes[fold] = 0;
	for(label = 0; label < n_labels; label++)
	  foldsizes[fold] += rows_per_fold_label(fold,label);
      }
      
      if (verbose) {
	for (fold=0; fold<blocks; fold++) {
	  cerr << "Fold " << fold << " total: " << foldsizes[fold] << " / ";
	  for (label = 0; label < n_labels; label++)
	    cerr << rows_per_fold_label(fold,label) << " ";
	cerr << endl;
	}
      }
      // initialize starting fold per label vector
      for(label = 0; label<n_labels; label++)
	fold_label[label] = 0;
      // initialize starting location per fold vector
      i = 0;
      for(fold = 0; fold<blocks; fold++)
      {
	location_fold[fold] = i;
	i += foldsizes[fold];
      }
      
      char ** new_records = (char **) malloc(n_records * sizeof(char *));
#ifdef DEBUG
      for(i=0; i<n_records; i++) 
	new_records[i] = 0;
#endif
      total_mem += (n_records * sizeof(char *));
      for(i=0; i<n_records; i++) {
	label = labels[i];
#ifdef DEBUG
	if (veryverbose)
	  cout << "N: " << i << " Label: " << label 
	       << " fold_label: " << fold_label[label]
	       << " location_fold: " <<  location_fold[fold_label[label]] 
	       << endl;

	if (new_records[location_fold[fold_label[label]]] != 0) {
	  cerr << "Assigning same place twice: " 
	       << new_records[location_fold[fold_label[label]]] << endl;
	  cerr << "Label:         " << label << endl;
	  cerr << "Fold_label:    " << fold_label[label] << endl;
	  cerr << "Location_Fold: " << location_fold[fold_label[label]] 
	       << endl;
	  exit(1);
	}

#endif
	
	new_records[location_fold[fold_label[label]]] 
	  = records[i];
#ifdef DEBUG
	if (rows_per_fold_label(fold_label[label],label) == 0) {
	  cerr << "rowsperfold already 0!!! label=" 
	       << label << " fold_label=" <<  fold_label[label] << endl;
	}
#endif
	j =  (rows_per_fold_label(fold_label[label],label) -= 1);
	location_fold[fold_label[label]] += 1;
	if (j == 0)
	  fold_label[label] += 1;
      }
      
      // now let records point to where new_records pointed,
      // but before delete the old array
      free(records);
      records = new_records;

#ifdef DEBUG
  for (i = 0; i < n_records; i++) {
    if (records[i]==0) {
      cerr << "INTERNAL ERROR 2: records["<<i<<"] is 0!\n";
      exit(2);
    }
  }
#endif  


      if (verbose) cerr << "Data stratified.\n";
    } else { // if doStrat
      // just split
      if (percent > 0.0) {
	if (percent >= 1.0) {
	  foldsizes[0] 
	    = (percent >= 1.0) ? percent : (n_records * percent);
	  foldsizes[1] = n_records - foldsizes[0];
	} else {
	  foldsizes[0] 
	    = (percent >= 1.0) ? percent : (n_records * percent);
	  foldsizes[1] = n_records - foldsizes[0];
	}
      } else {
	for (fold=0; fold<blocks; fold++) 
	  foldsizes[fold] = n_records / blocks;
	for (i=0; i<(n_records % blocks); i++)
	  foldsizes[i]++;
      }
    }

    // if -b specified, output training/test file
    if (testblock >= 0) {
      
      if (verbose) cerr << "Writing training file\n";
      // initialize starting location per fold vector again 
      // this got changed above
      i = 0;
      for(fold = 0; fold<blocks; fold++)
      {
	location_fold[fold] = i;
	i += foldsizes[fold];
      }

      if (outtrainfile != 0) os.open(outtrainfile);
      // if testblock is the first block, skip beginning part
      if (testblock != 0)
	outtrainfile ? 
	  outRecords(os,records,0,location_fold[testblock]) :
	outRecords(cout,records,0,location_fold[testblock]);
      // if testblock is the last block, skip trailing part
      if (testblock != (blocks-1))
	outtrainfile ?
	  outRecords(os,records,location_fold[testblock+1],n_records) :
	outRecords(cout,records,location_fold[testblock+1],n_records);
      if (outtrainfile != 0) os.close();

      
      // the testfile will only be written if at least one of 
      // training or testfile is specified (via -t or -e) 
      if (outtrainfile != 0 || outtestfile != 0) {
	if (verbose) cerr << "Writing test file\n";
	if (outtestfile != 0) os.open(outtestfile);
	outtestfile ?
	  outRecords(os,records,location_fold[testblock],
		     location_fold[testblock]+foldsizes[testblock]) :
	  outRecords(cout,records,location_fold[testblock],
		     location_fold[testblock]+foldsizes[testblock]);
	if (outtestfile != 0) os.close();
      }
    } else { // output the full stratified record table
      if (verbose) cerr << "Writing output file\n";
      if (outtrainfile != 0) os.open(outtrainfile);
      outtrainfile ?
	outRecords(os,records,0,n_records) :
	outRecords(cout,records,0,n_records);
      if (outtrainfile != 0) os.close();
    }
    
  } else { // if blocks > 1
    // output full original records table
    if (verbose) cerr << "Writing output file\n";
    if (outtrainfile != 0) os.open(outtrainfile);
    outtrainfile ?
      outRecords(os,records,0,n_records) :
      outRecords(cout,records,0,n_records);
    if (outtrainfile != 0) os.close();
  }
  
  // clean up memory 
  for (i=0;i<n_records;i++)
    free(records[i]);
  free(records);

  if (outstatefile!=0) {
    if (verbose) cerr << "Writing random state file\n";
    rand.write_state(outstatefile);
  }
  
  if (verbose) cerr << "Done ... " << total_mem << " bytes used\n";
}

void outRecords(ostream & out, 
	   char ** records, 
	   unsigned int from, 
	   unsigned int to) {
  if (verbose)
    cerr << "Writing from " << from << " to " << (to-1) <<endl;
  for (unsigned int i = from; i<to; i++)
    out << records[i] << endl;
}

char *pLabel(char * p)
/*    ----------  */
{
    char * t = p + strlen(p);
    
    while ( t!=p && *t != ',' ) t--;
    if (*t == ',') t++;
    return t;
}












