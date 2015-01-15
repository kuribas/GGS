# $Id: opts.c++ 9037 2010-07-06 04:05:44Z mburo $

# GNU 
GCC = g++
GCC_FLAGS = 									\
  -pipe 									\
  -Wall -Wuninitialized -W -Wundef						\
  -O -g 								        

# 32 bit for database

# GCC_FLAGS = -Wall -O0 -fpedantic -pipe -g

# Fujitsu
FCC = FCC -D_BSD_SOURCE 
FCC_FLAGS = -O

# KAI
KCC = /usr/local/KAI/KCC.pu-3.4d-2/KCC_BASE/bin/KCC
KCC_FLAGS =									\
  -O4										\
  +K3										\
  --abstract_float								\
  --abstract_pointer								\
  --no_exceptions


# final setting
VER      = .gcc
CPP      = $(GCC)
CC       = $(GCC) 
CC_FLAGS = $(GCC_FLAGS)
  

LIB_DEF = -Dglibc

#
#CC=/home/igord/gcc/bin/c++
#CC_INCS = -I$(GSAHOME)/pkg/inc
#LD_ARGS = -L$(GSAHOME)/pkg/lib
