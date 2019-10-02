# -*- mode: makefile -*-
# -----------------------------------------------------------------
# Programmer(s): Radu Serban @ LLNL
#                David J. Gardner @ LLNL
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
# Makefile for  serial examples
#
# This file is generated from a template using variables
# set at configuration time. It can be used as a template for
# other user Makefiles.
# -----------------------------------------------------------------

SHELL = sh

prefix       = /spack/opt/spack/linux-rhel7-x86_64/gcc-7.3.0/sundials-4.1.0-zcot6t4hb6hbkydothsxlbfh5gbbbprp
exec_prefix  = /spack/opt/spack/linux-rhel7-x86_64/gcc-7.3.0/sundials-4.1.0-zcot6t4hb6hbkydothsxlbfh5gbbbprp
includedir   = /spack/opt/spack/linux-rhel7-x86_64/gcc-7.3.0/sundials-4.1.0-zcot6t4hb6hbkydothsxlbfh5gbbbprp/include
libdir       = /spack/opt/spack/linux-rhel7-x86_64/gcc-7.3.0/sundials-4.1.0-zcot6t4hb6hbkydothsxlbfh5gbbbprp/lib64

CPP      = /spack/opt/spack/linux-rhel7-x86_64/gcc-4.8.5/gcc-7.3.0-qemsqrcwkk52f6neef4kg5wvoucsroif/bin/gcc
CPPFLAGS = -O3 -DNDEBUG
CC       = /spack/opt/spack/linux-rhel7-x86_64/gcc-4.8.5/gcc-7.3.0-qemsqrcwkk52f6neef4kg5wvoucsroif/bin/gcc
CFLAGS   = -O3 -DNDEBUG
LDFLAGS  = 
LIBS     =  -lm /usr/lib64/librt.so -lsundials_sunmatrixdense -lsundials_sunmatrixband

LINKFLAGS = -Wl,-rpath,/spack/opt/spack/linux-rhel7-x86_64/gcc-7.3.0/sundials-4.1.0-zcot6t4hb6hbkydothsxlbfh5gbbbprp/lib64

# -----------------------------------------------------------------------------------------

LIBRARIES_LAPACK = -lsundials_sunlinsollapackdense -lsundials_sunlinsollapackband  
LINKFLAGS_LAPACK = ${LINKFLAGS}::

INCLUDES_KLU  = 
LIBRARIES_KLU = -lsundials_sunlinsolklu 
LINKFLAGS_KLU = ${LINKFLAGS}:

INCLUDES_SLUMT  = 
LIBRARIES_SLUMT = -lsundials_sunlinsolsuperlumt  
LINKFLAGS_SLUMT = ${LINKFLAGS}::

TMP_INCS  = ${includedir} ${INCLUDES_SLUMT} ${INCLUDES_KLU}
INCLUDES  = $(addprefix -I, ${TMP_INCS})
LIBRARIES = -lsundials_sunmatrixsparse -lsundials_nvecserial ${LIBS}

# -----------------------------------------------------------------------------------------

EXAMPLES =  test_sunmatrix_sparse
EXAMPLES_DEPENDENCIES =  test_sunmatrix sundials_nvector sundials_matrix

OBJECTS = ${EXAMPLES:=.o}
OBJECTS_DEPENDENCIES = ${EXAMPLES_DEPENDENCIES:=.o}

# LAPACK Examples
EXAMPLES_BL = 
EXAMPLES_DEPENDENCIES_BL = 

OBJECTS_BL = ${EXAMPLES_BL:=.o}
OBJECTS_DEPENDENCIES_BL = ${EXAMPLES_DEPENDENCIES_BL:=.o}

# KLU Examples
EXAMPLES_KLU = 
EXAMPLES_DEPENDENCIES_KLU = 

OBJECTS_KLU = ${EXAMPLES_KLU:=.o}
OBJECTS_DEPENDENCIES_KLU = ${EXAMPLES_DEPENDENCIES_KLU:=.o}

# SuperLU_MT Examples
EXAMPLES_SLUMT = 
EXAMPLES_DEPENDENCIES_SLUMT = 

OBJECTS_SLUMT = ${EXAMPLES_SLUMT:=.o}
OBJECTS_DEPENDENCIES_SLUMT = ${EXAMPLES_DEPENDENCIES_SLUMT:=.o}

# -----------------------------------------------------------------------------------------

.SUFFIXES : .o .c

.c.o :
	${CC} ${CPPFLAGS} ${CFLAGS} ${INCLUDES} -c $<

# -----------------------------------------------------------------------------------------

all: examples examples_bl examples_klu examples_slumt

examples: ${OBJECTS}
	@for i in ${EXAMPLES} ; do \
	  echo "${CC} -o $${i} $${i}.o ${OBJECTS_DEPENDENCIES} ${CFLAGS} ${LDFLAGS} ${INCLUDES} -L${libdir} ${LIBRARIES} ${LINKFLAGS}" ; \
	  ${CC} -o $${i} $${i}.o ${OBJECTS_DEPENDENCIES} ${CFLAGS} ${LDFLAGS} ${INCLUDES} -L${libdir} ${LIBRARIES} ${LINKFLAGS} ; \
	done

examples_bl: ${OBJECTS_BL}
	@for i in ${EXAMPLES_BL} ; do \
	  echo "${CC} -o $${i} $${i}.o ${OBJECTS_DEPENDENCIES_BL} ${CFLAGS} ${LDFLAGS} ${INCLUDES} -L${libdir} ${LIBRARIES} ${LIBRARIES_LAPACK} ${LINKFLAGS_LAPACK}" ; \
	  ${CC} -o $${i} $${i}.o ${OBJECTS_DEPENDENCIES_BL} ${CFLAGS} ${LDFLAGS} ${INCLUDES} -L${libdir} ${LIBRARIES} ${LIBRARIES_LAPACK} ${LINKFLAGS_LAPACK} ; \
	done

examples_klu: ${OBJECTS_KLU}
	@for i in ${EXAMPLES_KLU} ; do \
	  echo "${CC} -o $${i} $${i}.o ${OBJECTS_DEPENDENCIES_KLU} ${CFLAGS} ${LDFLAGS} ${INCLUDES} -L${libdir} ${LIBRARIES} ${LIBRARIES_KLU} ${LINKFLAGS_KLU}" ; \
	  ${CC} -o $${i} $${i}.o ${OBJECTS_DEPENDENCIES_KLU} ${CFLAGS} ${LDFLAGS} ${INCLUDES} -L${libdir} ${LIBRARIES} ${LIBRARIES_KLU} ${LINKFLAGS_KLU} ; \
	done

examples_slumt: ${OBJECTS_SLUMT}
	@for i in ${EXAMPLES_SLUMT} ; do \
	  echo "${CC} -o $${i} $${i}.o ${OBJECTS_DEPENDENCIES_SLUMT} ${CFLAGS} ${LDFLAGS} ${INCLUDES} -L${libdir} ${LIBRARIES} ${LIBRARIES_SLUMT} ${LINKFLAGS_SLUMT}" ; \
	  ${CC} -o $${i} $${i}.o ${OBJECTS_DEPENDENCIES_SLUMT} ${CFLAGS} ${LDFLAGS} ${INCLUDES} -L${libdir} ${LIBRARIES} ${LIBRARIES_SLUMT} ${LINKFLAGS_SLUMT} ; \
	done

${OBJECTS}: ${OBJECTS_DEPENDENCIES}

${OBJECTS_BL}: ${OBJECTS_DEPENDENCIES_BL}

${OBJECTS_KLU}: ${OBJECTS_DEPENDENCIES_KLU}

${OBJECTS_SLUMT}: ${OBJECTS_DEPENDENCIES_SLUMT}

clean:
	rm -f ${OBJECTS_DEPENDENCIES} ${OBJECTS_DEPENDENCIES_BL} ${OBJECTS_DEPENDENCIES_KLU} ${OBJECTS_DEPENDENCIES_SLUMT}
	rm -f ${OBJECTS} ${OBJECTS_BL} ${OBJECTS_KLU} ${OBJECTS_SLUMT}
	rm -f ${EXAMPLES} ${EXAMPLES_BL} ${EXAMPLES_KLU} ${EXAMPLES_SLUMT}

# -----------------------------------------------------------------------------------------
