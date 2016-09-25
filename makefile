##########################################################
# 
# Copyright (c) 2012 Simon Southwell. All rights reserved.
#
# Date: 25th August 2012  
#
# Makefile for 'cpu8051' instruction set simulator
# 
# $Id: makefile,v 1.4 2014-01-25 15:38:38 simon Exp $
# $Source: /home/simon/CVS/src/cpu/8051/makefile,v $
# 
##########################################################

##########################################################
# Definitions
##########################################################

BASENAME=8051
TARGET=cpu${BASENAME}

LIBTARGET=libcpu8051.a
LIBSOTARGET=libcpu8051.so

LIBOBJS=execute.o inst.o read_ihx.o
OBJECTS=cpu8051.o ${LIBOBJS}

SRCDIR=./src
OBJDIR=./obj
TESTDIR=./test

LCOVINFO=cpu8051.info
COVLOGFILE=cov.log
COVDIR=cov_html
COVEXCL=${TARGET}.c 

OSTYPE:=$(shell uname -o)

ARCHOPT=

CC=gcc

# GCC in CYGWIN gives tedious warnings that all code is relocatable, and
# so -fPIC not required. So shut it up.
ifeq (${OSTYPE}, Cygwin)
  COPTS=${ARCHOPT} -g -I.
else
  COPTS=${ARCHOPT} -g -fPIC -I.
endif

COVOPTS=

##########################################################
# Dependency definitions
##########################################################

all : ${TARGET} ${LIBTARGET}

${OBJDIR}/cpu8051.o  : ${SRCDIR}/cpu8051.c ${SRCDIR}/cpu8051.h ${SRCDIR}/8051.h
${OBJDIR}/execute.o  : ${SRCDIR}/execute.c ${SRCDIR}/read_ihx.h
${OBJDIR}/inst.o     : ${SRCDIR}/8051.h
${OBJDIR}/read_ihx.o : ${SRCDIR}/read_ihx.c

##########################################################
# Compilation rules
##########################################################

${TARGET} : ${OBJECTS:%=${OBJDIR}/%} ${LIBSOTARGET} ${LIBTARGET}
	@$(CC) ${OBJECTS:%=${OBJDIR}/%} ${ARCHOPT} ${COVOPTS} -o ${TARGET}

${LIBTARGET}: ${LIBOBJS:%=${OBJDIR}/%}
	@rm -f ${LIBTARGET}
	@ar rcs $@ ${LIBOBJS:%=${OBJDIR}/%}

${LIBSOTARGET}: ${LIBOBJS:%=${OBJDIR}/%}
	@rm -f ${LIBSOTARGET}
	@$(CC) -shared -Wl,-soname,${LIBSOTARGET} -o ${LIBSOTARGET} ${COVOPTS} ${LIBOBJS:%=${OBJDIR}/%}

${OBJDIR}/%.o : ${SRCDIR}/%.c
	@$(CC) $(COPTS) ${COVOPTS} -c $< -o $@ 

##########################################################
# Microsoft Visual C++ 2010
##########################################################

MSVCDIR=./msvc
MSVCCONF="Release"

mscv_dummy:

MSVC:   mscv_dummy
	@MSBuild.exe ${MSVCDIR}/${BASENAME}.sln /nologo /v:q /p:Configuration=${MSVCCONF} /p:OutDir='..\..\'
	@rm -f *.pdb *.ilk

##########################################################
# Test
##########################################################

test: run_test
run_test: ${TARGET}
	@./runtest

##########################################################
# coverage
##########################################################

coverage:
	@lcov -c -d ${OBJDIR} -o ${LCOVINFO} > ${COVLOGFILE}
	@lcov -r ${LCOVINFO} ${COVEXCL} -o ${LCOVINFO} >> ${COVLOGFILE}
	@genhtml -o ${COVDIR} ${LCOVINFO} >> ${COVLOGFILE}

##########################################################
# Clean up rules
##########################################################

clean:
	@/bin/rm -rf ${TARGET} ${LIBTARGET} ${LIBSOTARGET} \
	${OBJDIR}/* \
	${TESTDIR}/*/*.ihx \
	${TESTDIR}/*/*.lst \
	${TESTDIR}/*/*.rel \
	${TESTDIR}/*/*.sym \
	${TESTDIR}/*/*~ \
	${COVDIR} *.info

cleanmsvc:
	@/bin/rm -rf ${MSVCDIR}/*.sdf \
	${MSVCDIR}/*.suo \
	${MSVCDIR}/${BASENAME}/*.vcxproj.user \
	${MSVCDIR}/Debug \
	${MSVCDIR}/Release \
	${MSVCDIR}/ipch \
	${MSVCDIR}/${BASENAME}/Debug \
	${MSVCDIR}/${BASENAME}/Release

sparkle: clean cleanmsvc
	@rm -f *.exe *.log
