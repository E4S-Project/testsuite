# -*- mode: makefile -*-
# -----------------------------------------------------------------
# Programmer: Radu Serban @ LLNL
# -----------------------------------------------------------------
# SUNDIALS Copyright Start
# Copyright (c) 2002-2019, Lawrence Livermore National Security
# and Southern Methodist University.
# All rights reserved.
#
# See the top-level LICENSE and NOTICE files for details.
#
# SPDX-License-Identifier: BSD-3-Clause
# SUNDIALS Copyright End
# -----------------------------------------------------------------
# Makefile for  parallel examples
#
# This file is generated from a template using various variables
# set at configuration time. It can be used as a template for
# other user Makefiles.
# -----------------------------------------------------------------

SHELL = sh

prefix       = /spack/opt/spack/linux-rhel7-x86_64/gcc-7.3.0/sundials-4.1.0-zcot6t4hb6hbkydothsxlbfh5gbbbprp
exec_prefix  = /spack/opt/spack/linux-rhel7-x86_64/gcc-7.3.0/sundials-4.1.0-zcot6t4hb6hbkydothsxlbfh5gbbbprp
includedir   = /spack/opt/spack/linux-rhel7-x86_64/gcc-7.3.0/sundials-4.1.0-zcot6t4hb6hbkydothsxlbfh5gbbbprp/include
libdir       = /spack/opt/spack/linux-rhel7-x86_64/gcc-7.3.0/sundials-4.1.0-zcot6t4hb6hbkydothsxlbfh5gbbbprp/lib64

CPP         = /spack/opt/spack/linux-rhel7-x86_64/gcc-4.8.5/gcc-7.3.0-qemsqrcwkk52f6neef4kg5wvoucsroif/bin/gcc
CPPFLAGS    = -O3 -DNDEBUG
CC          = /spack/opt/spack/linux-rhel7-x86_64/gcc-4.8.5/gcc-7.3.0-qemsqrcwkk52f6neef4kg5wvoucsroif/bin/gcc
CFLAGS      = -O3 -DNDEBUG
LDFLAGS     = 
LIBS        =  -lm /usr/lib64/librt.so

LINKFLAGS = -Wl,-rpath,/spack/opt/spack/linux-rhel7-x86_64/gcc-7.3.0/sundials-4.1.0-zcot6t4hb6hbkydothsxlbfh5gbbbprp/lib64

MPICC       = /spack/opt/spack/linux-rhel7-x86_64/gcc-7.3.0/mpich-3.2.1-lcdvbejz7w7ohecffa2qdng7y6dfxbab/bin/mpicc
MPI_INC_DIR = .
MPI_LIB_DIR = .
MPI_LIBS    = 
MPI_FLAGS   = 

TMP_INCS  = ${includedir} ${MPI_INC_DIR}
INCLUDES  = $(addprefix -I, ${TMP_INCS})
LIBRARIES = -lsundials_sunlinsolspbcgs -lsundials_nvecparallel ${LIBS}

EXAMPLES =  test_sunlinsol_spbcgs_parallel
EXAMPLES_DEPENDENCIES =  test_sunlinsol sundials_nvector sundials_linearsolver

OBJECTS = ${EXAMPLES:=.o}
OBJECTS_DEPENDENCIES = ${EXAMPLES_DEPENDENCIES:=.o}

# -----------------------------------------------------------------------------------------

.SUFFIXES : .o .c

.c.o :
	${MPICC} ${CPPFLAGS} ${CFLAGS} ${MPI_FLAGS} ${INCLUDES} -c $<

# -----------------------------------------------------------------------------------------

all: ${OBJECTS}
	@for i in ${EXAMPLES} ; do \
	  echo "${MPICC} -o $${i} $${i}.o ${OBJECTS_DEPENDENCIES} ${MPI_FLAGS} ${CFLAGS} ${LDFLAGS} ${INCLUDES} -L${libdir} ${LIBRARIES} -L${MPI_LIB_DIR} ${MPI_LIBS} ${LINKFLAGS}" ; \
	  ${MPICC} -o $${i} $${i}.o ${OBJECTS_DEPENDENCIES} ${MPI_FLAGS} ${CFLAGS} ${LDFLAGS} ${INCLUDES} -L${libdir} ${LIBRARIES} -L${MPI_LIB_DIR} ${MPI_LIBS} ${LINKFLAGS} ; \
	done

${OBJECTS}: ${OBJECTS_DEPENDENCIES}

clean:
	rm -f ${OBJECTS_DEPENDENCIES}
	rm -f ${OBJECTS}
	rm -f ${EXAMPLES}

# -----------------------------------------------------------------------------------------
