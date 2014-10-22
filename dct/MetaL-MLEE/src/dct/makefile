# 
#  $Source /user1/grimmer/clem/src/achim/src/RCS/makefile,v $
#  $Author: johann $
#  $Date: 2002/09/17 19:11:34 $
#  $Revision: 1.1.1.1.2.1 $
#
#/*
# *  $Log: makefile,v $
# *  Revision 1.1.1.1.2.1  2002/09/17 19:11:34  johann
# *  *** empty log message ***
# *
# *  Revision 1.1.1.1  2002/06/30 18:02:16  johann
# *  Initial CVS version
# *
# *  Revision 4.3  1998/07/17 12:07:31  kleiner
# *  *** empty log message ***
# *
# Revision 4.2  1998/07/17  12:01:57  kleiner
# added clem
#
# Revision 4.1  1998/07/17  11:53:29  kleiner
# switching to revision 4.1
#
# Revision 3.6  1998/07/15  08:17:06  spuhl
# files.c removed
#
# Revision 3.5  1998/07/02  08:24:32  spuhl
# *** empty log message ***
#
# Revision 3.4  1998/06/29  11:11:40  spuhl
# changed analist -> dct
#
# Revision 3.3  1998/06/29  06:33:40  spuhl
# added results.c
#
# Revision 3.2  1998/06/25  07:09:41  kleiner
# added getopt
#
# Revision 3.1  1998/06/23  12:41:36  kleiner
# *** empty log message ***
#
# Revision 2.8  1998/06/22  12:31:52  kleiner
# added dcdflib.c and ipmpar.c
#
# Revision 2.7  1998/06/04  10:50:09  kleiner
# added stat.c
#
# Revision 2.6  1998/05/18  15:25:57  kleiner
# removed some not necessary header-defs
#
# Revision 2.5  1998/05/18  07:04:05  kleiner
# *** empty log message ***
#
# Revision 2.4  1998/05/17  17:56:11  Administrator
# ?
#
# *  Revision 2.3  1998/05/17 17:46:27  Administrator
# *  changed make depend to be more uniform
# *
# *  Revision 2.2  1998/05/17 17:39:19  Administrator
# *  *** empty log message ***
# *
# *  Revision 2.1  1998/05/15 07:13:42  kleiner
# *  *** empty log message ***
# *
# Revision 1.4  1998/05/07  16:19:06  kleiner
# *** empty log message ***
#
# Revision 1.3  1998/05/07  09:07:24  kleiner
# added -f (clean)
#
# Revision 1.2  1998/05/07  08:46:31  kleiner
# include $Log: makefile,v $
# include Revision 1.1.1.1.2.1  2002/09/17 19:11:34  johann
# include *** empty log message ***
# include
# include Revision 1.1.1.1  2002/06/30 18:02:16  johann
# include Initial CVS version
# include
# include Revision 4.3  1998/07/17 12:07:31  kleiner
# include *** empty log message ***
# include
# Revision 4.2  1998/07/17  12:01:57  kleiner
# added clem
#
# Revision 4.1  1998/07/17  11:53:29  kleiner
# switching to revision 4.1
#
# Revision 3.6  1998/07/15  08:17:06  spuhl
# files.c removed
#
# Revision 3.5  1998/07/02  08:24:32  spuhl
# *** empty log message ***
#
# Revision 3.4  1998/06/29  11:11:40  spuhl
# changed analist -> dct
#
# Revision 3.3  1998/06/29  06:33:40  spuhl
# added results.c
#
# Revision 3.2  1998/06/25  07:09:41  kleiner
# added getopt
#
# Revision 3.1  1998/06/23  12:41:36  kleiner
# *** empty log message ***
#
# Revision 2.8  1998/06/22  12:31:52  kleiner
# added dcdflib.c and ipmpar.c
#
# Revision 2.7  1998/06/04  10:50:09  kleiner
# added stat.c
#
# Revision 2.6  1998/05/18  15:25:57  kleiner
# removed some not necessary header-defs
#
# Revision 2.5  1998/05/18  07:04:05  kleiner
# *** empty log message ***
#
# Revision 2.4  1998/05/17  17:56:11  Administrator
# ?
#
# include Revision 2.3  1998/05/17 17:46:27  Administrator
# include changed make depend to be more uniform
# include
# include Revision 2.2  1998/05/17 17:39:19  Administrator
# include *** empty log message ***
# include
# include Revision 2.1  1998/05/15 07:13:42  kleiner
# include *** empty log message ***
# include
# Revision 1.4  1998/05/07  16:19:06  kleiner
# *** empty log message ***
#
# Revision 1.3  1998/05/07  09:07:24  kleiner
# added -f (clean)
#
# added -Wall to CFLAGS
# changed clear to clean
#
# */

############################
###  Mafile for analist  ###
############################

CC	= gcc
# CFLAGS	= -g -pedantic -Wall -O9 -funroll-loops -O9
CFLAGS	= -g -pedantic -Wall -funroll-loops
EFILE   = dct
SOURCES = \
	dct.c		entropia.c	estatistica.c	domain.c	instances.c \
			str.c 		vector.c 	utils.c 	matrix.c    \
	sym_num.c 	quantile.c 	bhep_test.c 	multi_kor.c 	memory.c    \
	crit_val.c 	v_statistik.c  	helpfunc.c	inform.c	eigenv.c    \
	statistic.c     m_statistic.c	data_structures.c		results.c   \
	dcdflib.c

OBJS	= \
	dct.o		entropia.o	estatistica.o	domain.o	instances.o \
			str.o 		vector.o 	utils.o 	matrix.o    \
	sym_num.o 	quantile.o 	bhep_test.o 	multi_kor.o 	memory.o    \
	crit_val.o 	v_statistik.o  	helpfunc.o	inform.o	eigenv.o    \
	statistic.o     m_statistic.o	data_structures.o		results.o   \
	stat.o	dcdflib.o	ipmpar.o	getopt.o
		                                 
############################
#dependencies

$(EFILE): $(OBJS)
	$(CC) $(CFLAGS) -o $(EFILE) $(OBJS) -lm
############################

dct.o: dct.c         
	$(CC) $(CFLAGS) -c dct.c

stat.o: stat.c
	$(CC) $(CFLAGS) -c stat.c

getopt.o: getopt.c
	$(CC) $(CFLAGS) -c getopt.c

dcdflib.o: dcdflib.c
	$(CC) $(CFLAGS) -c dcdflib.c

ipmpar.o: ipmpar.c
	$(CC) $(CFLAGS) -c ipmpar.c

entropia.o: entropia.c 
	$(CC) $(CFLAGS) -c entropia.c

estatistica.o: estatistica.c 
	$(CC) $(CFLAGS) -c estatistica.c

data_structures.o:data_structures.c
	$(CC) $(CFLAGS) -c data_structures.c

domain.o: domain.c 
	$(CC) $(CFLAGS) -c domain.c

instances.o: instances.c
	$(CC) $(CFLAGS) -c instances.c

# files.o: files.c
#	$(CC) $(CFLAGS) -c files.c

str.o:str.c
	$(CC) $(CFLAGS) -c str.c

vector.o:vector.c
	$(CC) $(CFLAGS) -c vector.c

utils.o:utils.c
	$(CC) $(CFLAGS) -c utils.c

matrix.o: matrix.c
	$(CC) $(CFLAGS) -c matrix.c

quantile.o: quantile.c 
	$(CC) $(CFLAGS) -c quantile.c

multi_kor.o: multi_kor.c 
	$(CC) $(CFLAGS) -c multi_kor.c

memory.o: memory.c 
	$(CC) $(CFLAGS) -c memory.c

bhep_test.o: bhep_test.c 
	$(CC) $(CFLAGS) -c bhep_test.c

crit_val.o: crit_val.c 
	$(CC) $(CFLAGS) -c crit_val.c 

v_statistik.o: v_statistik.c 
	$(CC) $(CFLAGS) -c v_statistik.c

sym_num.o: sym_num.c
	$(CC) $(CFLAGS) -c sym_num.c

helpfunc.o: helpfunc.c 
	$(CC) $(CFLAGS) -c helpfunc.c

inform.o: inform.c 
	$(CC) $(CFLAGS) -c inform.c

statistic.o: statistic.c 
	$(CC) $(CFLAGS) -c statistic.c

m_statistic.o: m_statistic.c 
	$(CC) $(CFLAGS) -c m_statistic.c

eigenv.o: eigenv.c
	$(CC) $(CFLAGS) -c eigenv.c

results.o: results.c
	$(CC) $(CFLAGS) -c results.c


clean:
	rm -f *.o core $(EFILE)


depend:
	$(CC) -M $(SOURCES) > Makedeps;


all: dct
	strip dct

