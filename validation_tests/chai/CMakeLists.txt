##############################################################################
# Copyright (c) 2016-20, Lawrence Livermore National Security, LLC and CHAI
# project contributors. See the COPYRIGHT file for details.
#
# SPDX-License-Identifier: BSD-3-Clause
##############################################################################

#cmake_minimum_required(VERSION 3.14)
cmake_minimum_required(VERSION 3.21)

cmake_policy(VERSION 3.21...${CMAKE_VERSION})

project(Chai-Examples LANGUAGES C CXX VERSION 2022.03.0)

# 1. Standard CMake replacement for set(BLT_CXX_STD c++14) (breaks recent chai, don't use)
#set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 2. Replicate BLT's behavior by sending all executables to build/bin/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "PGI")
  message(STATUS "Setting CMAKE_CXX_EXTENSIONS to ON for PGI Compiler")
  set(CMAKE_CXX_EXTENSIONS ON)
endif()

message(STATUS "Using CMake version ${CMAKE_VERSION}")

# 3. Setup standard Options (can be toggled from compile.sh)
option(CHAI_ENABLE_CUDA "Enable CUDA support" OFF)
option(CHAI_ENABLE_HIP "Enable HIP support" OFF)
option(CHAI_ENABLE_MANAGED_PTR "Enable Managed Ptr" OFF)

if (CHAI_ENABLE_CUDA)
  if(NOT DEFINED CMAKE_CUDA_HOST_COMPILER)
    set(CMAKE_CUDA_HOST_COMPILER ${CMAKE_CXX_COMPILER})
  endif()
  enable_language(CUDA)
  set(CMAKE_CUDA_ARCHITECTURES $ENV{SPACK_CUDA_ARCH})
  set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} --expt-extended-lambda")
endif ()

# 4. Find pre-installed Spack packages natively
find_package(chai REQUIRED)
find_package(umpire REQUIRED) # Added because you link umpire below

# 5. Build Executables natively
# ---------------------------------------------------------
# chai-umpire-example.exe
# ---------------------------------------------------------
add_executable(chai-umpire-example.exe chai-umpire-allocators.cpp)
target_link_libraries(chai-umpire-example.exe PRIVATE chai umpire)
if (CHAI_ENABLE_CUDA)
  set_source_files_properties(chai-umpire-allocators.cpp PROPERTIES LANGUAGE CUDA)
endif()

# ---------------------------------------------------------
# chai-example.exe & pinned.exe
# ---------------------------------------------------------
if (CHAI_ENABLE_CUDA OR CHAI_ENABLE_HIP)
  add_executable(chai-example.exe example.cpp)
  target_link_libraries(chai-example.exe PRIVATE chai umpire)

  add_executable(pinned.exe pinned.cpp)
  target_link_libraries(pinned.exe PRIVATE chai umpire)

  if (CHAI_ENABLE_CUDA)
    set_source_files_properties(example.cpp pinned.cpp PROPERTIES LANGUAGE CUDA)
  endif()
endif ()

# ---------------------------------------------------------
# managed_ptr_example.exe
# ---------------------------------------------------------
if (CHAI_ENABLE_MANAGED_PTR)
  add_executable(managed_ptr_example.exe managed_ptr_example.cpp)
  target_link_libraries(managed_ptr_example.exe PRIVATE chai umpire)

  if (CHAI_ENABLE_CUDA)
    set_source_files_properties(managed_ptr_example.cpp PROPERTIES LANGUAGE CUDA)
  endif()
endif ()
