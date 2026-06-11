##############################################################################
# Copyright (c) 2016-24, Lawrence Livermore National Security, LLC and Umpire
# project contributors. See the COPYRIGHT file for details.
#
# SPDX-License-Identifier: (MIT)
##############################################################################

cmake_minimum_required(VERSION 3.14)

# Silence the CMP0144 warning for <PACKAGE>_ROOT variables on newer CMake versions
if(POLICY CMP0144)
  cmake_policy(SET CMP0144 NEW)
endif()

project(umpire_examples LANGUAGES C CXX)


# Replicate BLT's behavior by sending all executables to build/bin/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Configure build options
option(UMPIRE_ENABLE_CUDA "Enable CUDA support" OFF)
option(UMPIRE_ENABLE_HIP "Enable HIP support" OFF)
option(UMPIRE_ENABLE_C "Enable C support" OFF)
option(UMPIRE_ENABLE_DEVICE_ALLOCATOR "Enable device allocator" OFF)
option(UMPIRE_ENABLE_OPENMP_TARGET "Enable OpenMP target" OFF)

if (UMPIRE_ENABLE_CUDA)
  enable_language(CUDA)
  set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} --expt-extended-lambda")
endif()

# Find pre-installed Umpire
find_package(umpire REQUIRED)

# Need GNUInstallDirs to define standard variables like CMAKE_INSTALL_BINDIR
include(GNUInstallDirs)

set(umpire_examples "")

# --- Helper macro to easily generate standard C++ examples ---
macro(add_ump_example NAME SOURCE)
  add_executable(${NAME} ${SOURCE})
  target_link_libraries(${NAME} PRIVATE umpire)
  if(UMPIRE_ENABLE_CUDA)
    set_source_files_properties(${SOURCE} PROPERTIES LANGUAGE CUDA)
  endif()
  list(APPEND umpire_examples ${NAME})
endmacro()

# --- Standard Executables ---
add_ump_example(aligned_allocator aligned_allocator.cpp)
add_ump_example(alloc_access allocator_access.cpp)
add_ump_example(malloc malloc.cxx)
add_ump_example(get_parent get_parent.cpp)
add_ump_example(strategy_example strategy_example.cpp)
add_ump_example(allocator allocator.cxx)
add_ump_example(vector_allocator vector_allocator.cpp)
add_ump_example(size_limiter size_limiter.cpp)
add_ump_example(use_after_free use_after_free.cpp)

# Backtrace Example requires -rdynamic
add_ump_example(backtrace_example backtrace_example.cpp)
if (NOT WIN32 AND NOT APPLE)
  # Standard CMake replacement for blt_add_target_link_flags
  target_link_options(backtrace_example PRIVATE -rdynamic)
endif()

# --- C Executable ---
if (UMPIRE_ENABLE_C)
  add_executable(allocator_c allocator.c)
  target_link_libraries(allocator_c PRIVATE umpire)
  list(APPEND umpire_examples allocator_c)
endif()

# --- HIP Specific Executables ---
if (UMPIRE_ENABLE_HIP)
  if (UMPIRE_ENABLE_DEVICE_ALLOCATOR)
    add_executable(device_allocator_example device-allocator.cpp)
    target_link_libraries(device_allocator_example PRIVATE umpire umpire_device)
    
    # Apply -fgpu-rdc natively
    target_compile_options(device_allocator_example PRIVATE -fgpu-rdc)
    target_link_options(device_allocator_example PRIVATE -fgpu-rdc)
    
    list(APPEND umpire_examples device_allocator_example)
  endif()

  add_executable(multi_device multi_device.cpp)
  target_link_libraries(multi_device PRIVATE umpire)
  list(APPEND umpire_examples multi_device)
endif()

# --- CUDA Specific Executables ---
if (UMPIRE_ENABLE_CUDA)
  if (UMPIRE_ENABLE_DEVICE_ALLOCATOR)
    add_executable(device_allocator_example device-allocator.cpp)
    set_source_files_properties(device-allocator.cpp PROPERTIES LANGUAGE CUDA)
    target_link_libraries(device_allocator_example PRIVATE umpire umpire_device)
    
    # Native CMake equivalent of CUDA separable compilation
    set_target_properties(device_allocator_example PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
    
    list(APPEND umpire_examples device_allocator_example)
  endif()

  add_executable(multi_device multi_device.cpp)
  set_source_files_properties(multi_device.cpp PROPERTIES LANGUAGE CUDA)
  target_link_libraries(multi_device PRIVATE umpire)
  
  # Native CMake equivalent of CUDA separable compilation
  set_target_properties(multi_device PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
  
  list(APPEND umpire_examples multi_device)
endif()

# --- Install Targets ---
if (umpire_examples)
  install(TARGETS ${umpire_examples} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()
