#------------------------------------------------------------------------------
# Makefile for generating documentation.
# Needs doxygen (http://www.doxygen.org)
# $Id: Makefile 9037 2010-07-06 04:05:44Z mburo $
# $Source: /usr/bodo1/cvs/cvsggs/GGS/doxygen/Makefile,v $
#------------------------------------------------------------------------------

all: changelog doxygen

doxygen:
	mkdir -p html
	-cp changelog.txt html
	doxygen doxygen.cfg

changelog:
	-cd ..; doxygen/cvs2cl.pl -f doxygen/changelog.txt

.PHONY: all changelog doxygen
