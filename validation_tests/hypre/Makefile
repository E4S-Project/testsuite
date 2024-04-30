# Copyright (c) Lawrence Livermore National Security, LLC and other
#
# SPDX-License-Identifier: (Apache-2.0 OR MIT)

########################################################################
# Compiler and external dependences
########################################################################

CC = $(TEST_CC_MPI) 
CXX = $(TEST_CXX_MPI) 
CFLAGS = -I$(ADIOS2_ROOT)/include -Wall
CXXFLAGS = -I$(ADIOS2_ROOT)/include -Wall
LD = $(TEST_CXX) #$(TEST_CXX_MPI) #mpic++
LDFLAGS = -L$(ADIOS2_LIB_PATH) -ladios2_cxx11



#CC        = mpixlc
#CC        = mpicc
#CC        = mpiicc
#CC         = cc

#CXX       = mpixlC
#CXX       = mpic++
#CXX       = mpiicc
#CXX        = CC

LINK_CC   = ${CXX}
LINK_CXX  = ${CXX}

AR	= ar -rcu
RANLIB  = ranlib

LDFLAGS =
#LIBS    = -lm ${HYPRE_CUDA_LIBS} ${HYPRE_HIP_LIBS}

INCLUDES = $(HYPRE_ROOT)/include #${HYPRE_CUDA_INCLUDE} ${HYPRE_HIP_INCLUDE} ${MPIINCLUDE}

HYPRE_DIR = $(HYPRE_ROOT)

##################################################################
##  MPI options
##################################################################
#MPIPATH = /opt/cray/pe/mpich/8.1.21/ofi/crayclang/10.0
#MPIINCLUDE =  -I${MPIPATH}/include
#MPILIBDIRS = -L${MPIPATH}/lib
#MPILIBS    =  ${MPILIBDIRS} -lmpi  # -lmpi needed for AMD GPUs
#MPIFLAGS   =

########################################################################
# CUDA options
########################################################################
HYPRE_CUDA_PATH    = #/usr/tce/packages/cuda/cuda-10.1.243
HYPRE_CUDA_INCLUDE = #-I${HYPRE_CUDA_PATH}/include
HYPRE_CUDA_LIBS    = #-L${HYPRE_CUDA_PATH}/lib64 -lcudart -lcusparse -lcublas -lcurand

########################################################################
# HIP options
########################################################################
#HYPRE_HIP_PATH    = /opt/rocm-5.4.1
#HYPRE_HIP_INCLUDE = -I${HYPRE_HIP_PATH}/include
#HYPRE_HIP_LIBS    = -L${HYPRE_HIP_PATH}/lib -lamdhip64 -lrocsparse -lrocrand

########################################################################
# Compiling and linking options
########################################################################

CINCLUDES = -I. -I$(HYPRE_DIR)/include $(INCLUDES)
CDEFS = -DHYPRE_TIMING

########################################################################
# MPI only
########################################################################
COPTS = -O2 -DHAVE_CONFIG_H 
LINKOPTS = 

########################################################################
# MPI  and OpenMP threading
########################################################################
#COPTS = -O2 -DHAVE_CONFIG_H -fopenmp
#LINKOPTS = -fopenmp 
#COPTS = -O2 -DHAVE_CONFIG_H -qsmp=omp
#LINKOPTS = -qsmp=omp
#COPTS = -O2 -DHAVE_CONFIG_H -qopenmp
#LINKOPTS = -qopenmp 

CFLAGS = $(COPTS) $(CINCLUDES) $(CDEFS)
CXXOPTS = $(COPTS) -Wno-deprecated
CXXINCLUDES = $(CINCLUDES) -I..
CXXDEFS = $(CDEFS)
CXXFLAGS  = $(COPTS) $(CINCLUDES) $(CDEFS) 

LIBS = -L$(HYPRE_DIR)/lib -lHYPRE $(XLINK) ${MPILIBS} -lm $(HYPRE_CUDA_LIBS) $(HYPRE_HIP_LIBS)
#LIBS = -L$(HYPRE_DIR)/lib -lHYPRE $(XLINK) ${MPILIBS} $(HYPRE_HIP_LIBS)
#LFLAGS = $(LINKOPTS) $(LIBS) -lstdc++

XLINK = -Wl,-rpath,${HYPRE_DIR}/lib

LFLAGS = $(LINKOPTS) $(LIBS) $(XLINK)

########################################################################
# Rules for compiling the source files
########################################################################
.SUFFIXES: .c .f .cuf .cxx

.c.o:
	$(CC) $(CFLAGS) -c $<
.cuf.o:
	$(CUF) $(FFLAGS) $(CUFFLAGS) -c $<
.cxx.o:
	$(CXX) $(CXXFLAGS) -c $<

########################################################################
# List of all programs to be compiled
########################################################################
ALLPROGS = hypre-test

all: $(ALLPROGS)

default: all

gpu: all

########################################################################
# AMG driver
########################################################################
hypre-test: hypre-test.o
	$(LINK_CC) -o $@ $^ $(LFLAGS)

########################################################################
clean:
	rm -f $(ALLPROGS:=.o)
distclean: clean
	rm -f $(ALLPROGS) $(ALLPROGS:=*~)
