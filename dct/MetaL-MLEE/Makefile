# $Id: Makefile,v 1.2.2.2 2002/09/17 19:11:22 johann Exp $
# all: compile all that is there and copy binaries
#   into bin. This will compile and copy all of the following
# shuffle
# dct

# md5: this does not get compiled by default since
#   it is assumed to be installed in the system
# If you really need this, run "make md5" and 
# change your config.pm file (see manual for more info)


CP = /bin/cp
RM = /bin/rm

all: shuffle dct 

shuffle:
	cd src/shuffle; $(MAKE); $(CP) shuffle ../../bin

dct:
	cd src/dct; $(MAKE); $(CP) dct ../../bin

md5:
	cd src/md5; $(MAKE); $(CP) md5sum ../../bin

md5sum: md5


# OR:
#  clean: remove all object and binary files that are no
#    in the binprecompiled directories

clean:
	-$(RM) bin/*
	touch bin/.keepme
	-cd src/dct; $(MAKE) clean
	-cd src/shuffle; $(MAKE) clean
	-cd src/md5; $(MAKE) clean
	-cd doc; $(RM) *.aux *.dvi *.log *.toc *.bbl

distclean: clean
	-find . -name '*~' | xargs $(RM)

.PHONY: shuffle dct clean all

