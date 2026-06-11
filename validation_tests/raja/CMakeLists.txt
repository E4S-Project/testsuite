###############################################################################
# Copyright (c) 2016-19, Lawrence Livermore National Security, LLC
# and RAJA project contributors. See the RAJA/COPYRIGHT file for details.
#
# SPDX-License-Identifier: (BSD-3-Clause)
###############################################################################
cmake_minimum_required(VERSION 3.12) 

project(raja-example LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

option(ENABLE_CUDA "Enable CUDA support" OFF)
option(ENABLE_HIP "Enable HIP support" OFF)

if (ENABLE_CUDA)
  enable_language(CUDA)
  set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} --expt-extended-lambda")
endif ()

# Locate the Spack-installed RAJA package
find_package(RAJA REQUIRED)

add_subdirectory(src)
