##############################################################################
# Copyright (c) 2016-20, Lawrence Livermore National Security, LLC and CHAI
# project contributors. See the COPYRIGHT file for details.
#
# SPDX-License-Identifier: BSD-3-Clause
##############################################################################

cmake_policy(SET CMP0057 NEW)
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0025 NEW)

#include(CMakeDependentOption)
#include(CMakePackageConfigHelpers)

project(Chai-Examples LANGUAGES C CXX VERSION 2022.03.0)
cmake_minimum_required(VERSION 3.14)

#include(cmake/SetupChaiOptions.cmake)

set(BLT_CXX_STD c++14 CACHE STRING "")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "PGI")
  message(STATUS "Setting CMAKE_CXX_EXTENSIONS to ON for PGI Compiler")
  set( CMAKE_CXX_EXTENSIONS ON )
endif()

message(STATUS "Using CMake version ${CMAKE_VERSION}")


################################
# BLT
################################
if (NOT BLT_LOADED)
  if (DEFINED BLT_SOURCE_DIR)
    if (NOT EXISTS ${BLT_SOURCE_DIR}/SetupBLT.cmake)
        message(FATAL_ERROR "Given BLT_SOURCE_DIR does not contain SetupBLT.cmake")
    endif()
  else ()
    set (BLT_SOURCE_DIR ${PROJECT_SOURCE_DIR}/blt CACHE PATH "")

    if (NOT EXISTS ${BLT_SOURCE_DIR}/SetupBLT.cmake)
      message(FATAL_ERROR "\
      The BLT submodule is not present. \
      If in git repository run the following two commands:\n \
      git submodule init\n \
      git submodule update")
    endif ()
  endif ()

  include(${BLT_SOURCE_DIR}/SetupBLT.cmake)
endif()

include(cmake/ChaiBasics.cmake)
##############################
    list(APPEND CMAKE_PREFIX_PATH ${chai_DIR})
    find_package(chai REQUIRED)

set (chai_umpire_example_depends
  chai umpire )

if (CHAI_ENABLE_CUDA)
  set (chai_umpire_example_depends
    ${chai_umpire_example_depends}
    cuda)
endif()
if (CHAI_ENABLE_HIP)
  set (chai_umpire_example_depends
    ${chai_umpire_example_depends}
    blt::hip)
endif()

blt_add_executable(
  NAME chai-umpire-example.exe
  SOURCES chai-umpire-allocators.cpp
  DEPENDS_ON ${chai_umpire_example_depends})

if (CHAI_ENABLE_CUDA OR CHAI_ENABLE_HIP)
  blt_add_executable(
    NAME chai-example.exe
    SOURCES example.cpp
    DEPENDS_ON ${chai_umpire_example_depends})

  blt_add_executable(
    NAME pinned.exe
    SOURCES pinned.cpp
    DEPENDS_ON ${chai_umpire_example_depends})
endif ()

if (CHAI_ENABLE_MANAGED_PTR)
  blt_add_executable(
    NAME managed_ptr_example.exe
    SOURCES managed_ptr_example.cpp
    DEPENDS_ON ${chai_umpire_example_depends})
endif ()

