# -*- mode: cmake -*-
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
# CMakeLists.txt for IDA parallel examples
#
# This file is generated from a template using  various variables
# set at configuration time. It can be used as a template for
# other user CMakeLists configuration files.
# -----------------------------------------------------------------

cmake_minimum_required(VERSION 2.8)

# Specify project name
PROJECT(IDA_parallel_examples C)

MARK_AS_ADVANCED(EXECUTABLE_OUTPUT_PATH LIBRARY_OUTPUT_PATH)

# Set the names of the examples to be built
SET(examples  idaFoodWeb_kry_bbd_p idaFoodWeb_kry_p idaHeat2D_kry_bbd_p idaHeat2D_kry_p)

# Set names of examples specific dependency source files
SET(examples_dependencies )

# Set name of MPI compiler to be used (CC or MPICC)
SET(MPI_MPICC /spack/opt/spack/linux-rhel7-x86_64/gcc-7.3.0/mpich-3.2.1-lcdvbejz7w7ohecffa2qdng7y6dfxbab/bin/mpicc CACHE STRING "MPICC compiler script")
SET(CMAKE_C_COMPILER ${MPI_MPICC})

# Specify path to SUNDIALS header files
SET(SUNDIALS_INC_DIR
  /spack/opt/spack/linux-rhel7-x86_64/gcc-7.3.0/sundials-4.1.0-zcot6t4hb6hbkydothsxlbfh5gbbbprp/include
  CACHE STRING
  "Location of SUNDIALS header files")

# Add path to SUNDIALS header files
INCLUDE_DIRECTORIES(${SUNDIALS_INC_DIR})

# Set search path for SUNDIALS libraries 
SET(SUNDIALS_LIB_DIR /spack/opt/spack/linux-rhel7-x86_64/gcc-7.3.0/sundials-4.1.0-zcot6t4hb6hbkydothsxlbfh5gbbbprp/lib64)

# Find the SUNDIALS solver's library
FIND_LIBRARY(SUNDIALS_SOLVER_LIB
  sundials_ida ${SUNDIALS_LIB_DIR}
  DOC "IDA library")

# Find the NVECTOR library
FIND_LIBRARY(SUNDIALS_NVEC_LIB
  sundials_nvecparallel ${SUNDIALS_LIB_DIR}
  DOC "NVECTOR library")

# Set additional libraries
SET(SUNDIALS_EXTRA_LIB  -lm /usr/lib64/librt.so CACHE STRING "Additional libraries")

# List of all libraries
SET(SUNDIALS_LIBS ${SUNDIALS_SOLVER_LIB} ${SUNDIALS_NVEC_LIB} ${SUNDIALS_EXTRA_LIB})

# Build each example one by one
FOREACH(example ${examples})
  
  # example source files
  ADD_EXECUTABLE(${example} ${example}.c ${examples_dependencies})

  # libraries to link against
  TARGET_LINK_LIBRARIES(${example} ${SUNDIALS_LIBS})

ENDFOREACH(example ${examples})
