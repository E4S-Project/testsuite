##
## Copyright (c) 2018-2021, Carnegie Mellon University
## All rights reserved.
##
## See LICENSE file for full information
##

##  Sample external application that wants to use FFTX
##  define normal items needed in app

cmake_minimum_required ( VERSION 3.14 )

set(CMAKE_BUILD_TYPE Release CACHE STRING "Debug, Release, RelWithDebInfo, MinSizeRel")

set ( PROJECT dftbatlib_test )
set ( _build_for "CPU" )		##  Assume building for CPU

enable_language ( C )
enable_language ( CXX )

if ( WIN32 )
    list ( APPEND ADDL_COMPILE_FLAGS -D_USE_MATH_DEFINES )
    set ( CMAKE_CUDA_ARCHITECTURES 52 )		##  60 61 62 70 72 75 80 )
else ()
    set ( CMAKE_CUDA_ARCHITECTURES 60 61 62 70 72 75 80 )
endif ()

if ( APPLE )
    ##  Check if we're on Apple/MAC
    ##  Check if arch should be x86_64 or arm64
    message ( STATUS "Running on Apple / MAC" )
    message ( STATUS "CMAKE_OSX_ARCHITECTURES = ${CMAKE_OSX_ARCHITECTURES}" )
    message ( STATUS "CMAKE_SYSTEM_PROCESSOR = ${CMAKE_SYSTEM_PROCESSOR}" )
    if ( ${CMAKE_OSX_ARCHITECTURES} MATCHES "arm64" OR ${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64.*" )
	set ( ADDL_COMPILE_FLAGS -arch arm64 )
        message ( STATUS "Set addl compile flags: ${ADDL_COMPILE_FLAGS}" )
    elseif ( ${CMAKE_OSX_ARCHITECTURES} MATCHES "x86_64" OR ${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64.*")
	set ( ADDL_COMPILE_FLAGS -arch x86_64 )
        message ( STATUS "Set addl compile flags: ${ADDL_COMPILE_FLAGS}" )
    endif ()
endif ()	

include ( CheckLanguage )
check_language ( CUDA )
if ( CMAKE_CUDA_COMPILER )
    enable_language ( CUDA )
    find_package ( CUDAToolkit )
    if ( ${CUDAToolkit_FOUND} )
	message ( STATUS "CUDA Toolkit Found : Version = ${CUDAToolkit_VERSION}" )
	project ( ${PROJECT} LANGUAGES C CXX CUDA )
    else ()
	message ( STATUS "CUDA Toolkit NOT found -- only build CPU / serial example" )
	project ( ${PROJECT} LANGUAGES C CXX )
    endif ()
else ()
    message ( STATUS "No support for CUDA available" )
endif ()

##  check_language ( HIP )		## HIP support added in version 3.21 (not working on spock)
##  if ( CMAKE_HIP_COMPILER )
##      enable_language ( HIP )
##  else ()
##      message ( STATUS "No support for HIP available" )
##  endif ()

message ( STATUS "CMAKE_CXX_COMPILER = ${CMAKE_CXX_COMPILER}" )
if ( "${CMAKE_CXX_COMPILER}" MATCHES "hipcc$" )
    message ( STATUS "${CMAKE_CXX_COMPILER} matches 'hipcc$' " )
    ##  Setup what we need to build for HIP/ROCm
    list ( APPEND CMAKE_PREFIX_PATH /opt/rocm/hip /opt/rocm )
    find_package ( hip REQUIRED )
    if ( ${hip_FOUND} )
	##  HIP/ROCm support found
	message ( STATUS "Found HIP: Version = ${hip_VERSION}" )
    else ()
	message ( SEND_ERROR "HIP NOT FOUND: HIP is required to build")
    endif ()

    ##  Adjust include and library directories
    ##  Need to add $ROCM_PATH for includes and libraries
    if ( DEFINED ENV{ROCM_PATH} )
	message ( STATUS "ROCM_PATH is defined: $ENV{ROCM_PATH}" )
	include_directories ( $ENV{ROCM_PATH}/include/hipfft $ENV{ROCM_PATH}/include )
    endif ()
    list ( APPEND LIBS_FOR_HIP hipfft rocfft )
    list ( APPEND ADDL_COMPILE_FLAGS -DFFTX_HIP )
endif ()

set ( XFORMLIB_test transformlib_test )
set ( POISSON1_TEST poissonTest )
set ( PERF_FRAME_TEST perf_test_driver )
set ( BATCH1D_DRIVER batch1d_test_driver )

##  Set C standard 11
set ( CMAKE_C_STANDARD 11 )
set ( CMAKE_CXX_STANDARD 11 )

if ( ${CUDAToolkit_FOUND} )
    set ( CUDA_COMPILE_FLAGS -m64 -rdc=false )
    if ( WIN32 )
	set ( LIBS_FOR_CUDA cufft cuda nvrtc )
    else ()
	set ( LIBS_FOR_CUDA cufft culibos cuda nvrtc )
    endif ()
    list ( APPEND ADDL_COMPILE_FLAGS -DFFTX_CUDA )
endif ()

##  Setup and access any SPIRAL related items...
##  Get SPIRAL home... only need SPIRAL for some include files

if ( DEFINED ENV{SPIRAL_HOME} )
    message ( STATUS "SPIRAL_HOME = $ENV{SPIRAL_HOME}" )
    set ( SPIRAL_SOURCE_DIR $ENV{SPIRAL_HOME} )
else ()
    if ( "x${SPIRAL_HOME}" STREQUAL "x" )
        message ( FATAL_ERROR "SPIRAL_HOME environment variable undefined and not specified on command line" )
    endif ()
    set ( SPIRAL_SOURCE_DIR ${SPIRAL_HOME} )
endif ()

include_directories ( ${SPIRAL_SOURCE_DIR}/profiler/targets/include )

##  FFTX_HOME must be defined in the environment
if ( DEFINED ENV{FFTX_HOME} )
    message ( STATUS "FFTX_HOME = $ENV{FFTX_HOME}" )
    set ( FFTX_SOURCE_DIR $ENV{FFTX_HOME} )
else ()
    if ( "x${FFTX_HOME}" STREQUAL "x" )
        message ( FATAL_ERROR "FFTX_HOME environment variable undefined and not specified on command line" )
    endif ()
    set ( FFTX_SOURCE_DIR ${FFTX_HOME} )
endif ()

##  Include FFTX CMake functions
include ( "${FFTX_SOURCE_DIR}/CMakeIncludes/FFTXCmakeFunctions.cmake" )

##  FFTX_find_libraries () finds the FFTX libraries, paths, etc. and
##  exposes the following variables:
##
##  FFTX_LIB_INCLUDE_PATHS -- include paths for FFTX include & library headers
##  FFTX_LIB_NAMES         -- list of FFTX libraries
##  FFTX_LIB_LIBRARY_PATH  -- path to libraries (for linker)
##  
##  You don't need to call FFTX_find_libraries() directly unless you
##  specifically want access to the variables listed above -- its
##  called as part of FFTX_add_includes_libs_to_target ( _target )
##

FFTX_find_libraries ()
##  message ( STATUS "Include paths:   ${FFTX_LIB_INCLUDE_PATHS}" )
##  message ( STATUS "Libraries found: ${FFTX_LIB_NAMES}" )
##  message ( STATUS "Library path is: ${FFTX_LIB_LIBRARY_PATH}" )

##  Setup CMAKE_INSTALL_PREFIX to point to the top level source directory
set ( CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR} )
message ( STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}" )

##  set CMAKE_INSTALL_RPATH to point to FFTX library directory
set ( CMAKE_INSTALL_RPATH ${FFTX_LIB_LIBRARY_PATH} )
message ( STATUS "Defined: INSTALL_RPATH = ${CMAKE_INSTALL_RPATH}" )

if ( ${CUDAToolkit_FOUND} )
    add_executable              ( ${XFORMLIB_test} ${XFORMLIB_test}.cpp )
    set_source_files_properties ( ${XFORMLIB_test}.cpp PROPERTIES LANGUAGE CUDA )
    target_compile_options      ( ${XFORMLIB_test} PRIVATE ${CUDA_COMPILE_FLAGS} )
    set_property         ( TARGET ${XFORMLIB_test} PROPERTY CUDA_RESOLVE_DEVICE_SYMBOLS ON )
    target_link_libraries       ( ${XFORMLIB_test} PRIVATE ${LIBS_FOR_CUDA} )
    list ( APPEND _targets        ${XFORMLIB_test} )

    add_executable              ( ${PERF_FRAME_TEST} ${PERF_FRAME_TEST}.cpp )
    set_source_files_properties ( ${PERF_FRAME_TEST}.cpp PROPERTIES LANGUAGE CUDA )
    target_compile_options      ( ${PERF_FRAME_TEST} PRIVATE ${CUDA_COMPILE_FLAGS} )
    set_property         ( TARGET ${PERF_FRAME_TEST} PROPERTY CUDA_RESOLVE_DEVICE_SYMBOLS ON )
    target_link_libraries       ( ${PERF_FRAME_TEST} PRIVATE ${LIBS_FOR_CUDA} )
    list ( APPEND _targets        ${PERF_FRAME_TEST} )

    add_executable              ( ${BATCH1D_DRIVER} ${BATCH1D_DRIVER}.cpp )
    set_source_files_properties ( ${BATCH1D_DRIVER}.cpp PROPERTIES LANGUAGE CUDA )
    target_compile_options      ( ${BATCH1D_DRIVER} PRIVATE ${CUDA_COMPILE_FLAGS} )
    set_property         ( TARGET ${BATCH1D_DRIVER} PROPERTY CUDA_RESOLVE_DEVICE_SYMBOLS ON )
    target_link_libraries       ( ${BATCH1D_DRIVER} PRIVATE ${LIBS_FOR_CUDA} )
    list ( APPEND _targets        ${BATCH1D_DRIVER} )

    set ( _build_for "CUDA" )
endif ()

if ( ${hip_FOUND} )
    add_executable              ( ${XFORMLIB_test} ${XFORMLIB_test}.cpp )
    set_source_files_properties ( ${XFORMLIB_test}.cpp PROPERTIES LANGUAGE CXX )
    target_link_directories     ( ${XFORMLIB_test} PRIVATE $ENV{ROCM_PATH}/lib )
    target_link_libraries       ( ${XFORMLIB_test} PRIVATE ${LIBS_FOR_HIP} )
    list ( APPEND _targets        ${XFORMLIB_test} )

    add_executable              ( ${PERF_FRAME_TEST} ${PERF_FRAME_TEST}.cpp )
    set_source_files_properties ( ${PERF_FRAME_TEST}.cpp PROPERTIES LANGUAGE CXX )
    target_link_directories     ( ${PERF_FRAME_TEST} PRIVATE $ENV{ROCM_PATH}/lib )
    target_link_libraries       ( ${PERF_FRAME_TEST} PRIVATE ${LIBS_FOR_HIP} )
    list ( APPEND _targets        ${PERF_FRAME_TEST} )

    add_executable              ( ${BATCH1D_DRIVER} ${BATCH1D_DRIVER}.cpp )
    set_source_files_properties ( ${BATCH1D_DRIVER}.cpp PROPERTIES LANGUAGE CXX )
    target_link_directories     ( ${BATCH1D_DRIVER} PRIVATE $ENV{ROCM_PATH}/lib )
    target_link_libraries       ( ${BATCH1D_DRIVER} PRIVATE ${LIBS_FOR_HIP} )
    list ( APPEND _targets        ${BATCH1D_DRIVER} )

    set ( _build_for "HIP" )
endif ()

message ( STATUS "_build_for = ${_build_for}" )
if ( ${_build_for} STREQUAL "CPU" )
    ##  Add CPU only executables
    ##  add_executable ( ${PROJECT} ${PROJECT}.cpp )
    ##  set   ( _targets ${PROJECT} )

    add_executable          ( ${POISSON1_TEST} ${POISSON1_TEST}.cpp )
    target_link_libraries   ( ${POISSON1_TEST} PRIVATE dl )
    list ( APPEND _targets    ${POISSON1_TEST} )
endif ()

foreach ( _targ ${_targets} )
    FFTX_add_includes_libs_to_target ( ${_targ} )
endforeach ()

install ( TARGETS ${_targets} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
