# Copyright (c) 2017-2021, Lawrence Livermore National Security, LLC and
# other Axom Project Developers. See the top-level LICENSE file for details.
#
# SPDX-License-Identifier: (BSD-3-Clause)
#------------------------------------------------------------------------------
# Example that shows how to use an installed instance of Axom 
# in a CMake-based build system.
#
# Configuration variables are stored in a CMake cache file 'host-config.cmake'
# which defines paths to axom and possibly other TPLs.
# It also contains information about the compiler used to build axom.
#------------------------------------------------------------------------------
#
# To build:
#  mkdir build
#  cd build
#  cmake -C ../host-config.cmake ..
#  make
#  ./example
#
#------------------------------------------------------------------------------

#cmake_minimum_required(VERSION 3.10)
cmake_minimum_required(VERSION 3.21)

cmake_policy(VERSION 3.21...${CMAKE_VERSION})

project(using_with_cmake)

#------------------------------------------------------------------------------
# Check for AXOM_DIR and use CMake's find_package to import axom's targets
#------------------------------------------------------------------------------
if(NOT DEFINED AXOM_DIR OR NOT EXISTS ${AXOM_DIR}/lib/cmake/axom-config.cmake)
    message(FATAL_ERROR "Missing required 'AXOM_DIR' variable pointing to an installed axom")
endif()

find_package(axom REQUIRED
             NO_DEFAULT_PATH 
             PATHS ${AXOM_DIR}/lib/cmake)


if("$ENV{TEST_COMPILER_BACKEND}" STREQUAL "CUDA")
    message(STATUS "Test environment detected: CUDA. Forcing nvcc compilation.")
    set(CMAKE_CUDA_ARCHITECTURES $ENV{SPACK_CUDA_ARCH})
    set_source_files_properties(example.cpp PROPERTIES LANGUAGE CUDA)

elseif("$ENV{TEST_COMPILER_BACKEND}" STREQUAL "HIP")
    message(STATUS "Test environment detected: ROCm. Forcing hipcc compilation.")
    set_source_files_properties(example.cpp PROPERTIES LANGUAGE HIP)

else()
    message(STATUS "Test environment detected: Host-only CXX compilation.")
endif()

#------------------------------------------------------------------------------
# Set up example target that depends on axom
#------------------------------------------------------------------------------
add_executable(example example.cpp)

# setup the axom include path
target_include_directories(example PRIVATE ${AXOM_INCLUDE_DIRS})

# link to axom targets
target_link_libraries(example axom)
target_link_libraries(example axom::fmt)

