
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<mtcc.h>

RandomMT::RandomMT() 
{
 handle = 0; 
}
RandomMT::~RandomMT()
{ 
  if (handle!=0) destroyrand(handle);
}
void RandomMT::initialize() 
{
  if (handle!=0) destroyrand(handle);
  handle = sgenrand(0); 
}
void RandomMT::initialize(unsigned long seed)
{
  if (handle!=0) destroyrand(handle);
  handle = sgenrand(seed); 
}
unsigned long RandomMT::operator()(const unsigned long max)
{
  if (handle == 0) handle = sgenrand(0);
  return genrand(handle)%max;
}
unsigned long RandomMT::operator()()
{
  if (handle == 0) handle = sgenrand(0);
  return genrand(handle);
}
double RandomMT::getDouble()
{
  if (handle == 0) handle = sgenrand(0);
  return genrandr(handle);
}
double RandomMT::getDouble2()
{
  if (handle == 0) handle = sgenrand(0);
  return genrandr2(handle);
}
int RandomMT::read_state(const char * filename) 
{ 
  if (handle == 0) handle = sgenrand(0);
  FILE * f;
  if (0 == (f = fopen(filename, "r")))
    return errno;

  mt_read_randomstate(handle,f);
  fclose(f);
  return 0;
}
int RandomMT::write_state(const char * filename)
{
  if (handle == 0) handle = sgenrand(0);
  FILE * f;
  if (0 == (f = fopen(filename, "w")))
    return errno;

  mt_write_randomstate(handle,f);
  fclose(f);
  return 0;
}
int RandomMT::read_state() 
{ 
  if (handle == 0) handle = sgenrand(0);
  FILE * f = 0;
  mt_read_randomstate(handle,f);
  return 0;
}
int RandomMT::write_state()
{
  if (handle == 0) handle = sgenrand(0);
  FILE * f = 0; 
  mt_write_randomstate(handle,f);
  return 0;
}
