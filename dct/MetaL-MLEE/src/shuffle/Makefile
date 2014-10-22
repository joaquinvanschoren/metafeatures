MTPATH = .
CCC= g++
CC = gcc
# for DEBUG:
#OPTIONS = -v -g  -c -I$(MTPATH) -w -D_NO_NAMESPACE -DDEBUG -D__COMPILEDON__="\"`date`\""
# for no debug:
OPTIONS = -v -O4 -c -I$(MTPATH) -w -D_NO_NAMESPACE -D__COMPILEDON__="\"`date`\""
LNK = g++ -static
#.SUFFIXES:
#.SUFFIXES: .cc .o .c .h

TARGETS = shuffle  

all: $(TARGETS)

clean:
	/bin/rm -f *.o $(TARGETS)

%.o: %.cc
	$(CCC) $(OPTIONS) $< 

# make local objects for MT random number generator
mt.o: $(MTPATH)/mt.c $(MTPATH)/mt.h
	$(CC) $(OPTIONS) $(MTPATH)/mt.c
mtcc.o: $(MTPATH)/mtcc.cc $(MTPATH)/mtcc.h $(MTPATH)/mt.h
	$(CCC) $(OPTIONS) $(MTPATH)/mtcc.cc

# getopt substitute (not available on all systems)
mygetopt.o: mygetopt.cc

# program to generate random numbers/states
mtgen.o: mtgen.cc
mtgen: mtgen.o mygetopt.o mtcc.o mt.o
	$(LNK) $? -o $@


# shuffle data records randomly
shuffle.o: shuffle.cc 

shuffle: shuffle.o mygetopt.o mtcc.o mt.o
	$(LNK) $? -o $@
	strip shuffle

tests: 
	cd ../tests ; ./testit f01
