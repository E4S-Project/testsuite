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

prefix       = ${SUNDIALS_ROOT} 
exec_prefix  = ${SUNDIALS_ROOT} 
includedir   = ${SUNDIALS_ROOT}/include 
libdir       = ${SUNDIALS_ROOT}/lib64 

CPP         = gcc
CPPFLAGS    = -O3 -DNDEBUG
CC          = gcc
CFLAGS      = -O3 -DNDEBUG
LDFLAGS     = 
LIBS        =  -lm /usr/lib64/librt.so

LINKFLAGS = -Wl,-rpath,${libdir} 

MPICC       = mpicc
MPI_INC_DIR = .
MPI_LIB_DIR = .
MPI_LIBS    = 
MPI_FLAGS   = 

TMP_INCS  = ${includedir} ${MPI_INC_DIR}
INCLUDES  = $(addprefix -I, ${TMP_INCS})
LIBRARIES = -lsundials_nvecparallel -lsundials_nvecparallel ${LIBS}

EXAMPLES =  test_nvector_mpi
EXAMPLES_DEPENDENCIES =  test_nvector sundials_nvector

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
