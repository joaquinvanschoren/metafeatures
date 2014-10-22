#ifndef __MT19937CC_H
#define __MT19937CC_H

#define CPP
#include<mt.h>

class RandomMT
{
  public:
  RandomMT();
  ~RandomMT();
  void initialize();
  void initialize(unsigned long seed);
  unsigned long operator()(const unsigned long max);
  unsigned long operator()();
  double getDouble();
  double getDouble2();
  // all the following I/O function return errno if something
  // went wrong during open:
  int read_state(const char * filename); 
  int write_state(const char * filename);
  int read_state(); // read from std-in  
  int write_state(); //write to std-out
  private:
  MTRANDHANDLE handle;
};
#endif
