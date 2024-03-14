#!/bin/bash  
. ./setup.sh

#if ! command -v cmake >/dev/null 2>&1 ; then
#  spackLoadUnique cmake
#fi
set -e
set -x
cd build_cmake_installed
unset KOKKOS_ROOT
#export CXX=`which $TEST_CXX`
mkdir -p build
cd build
#CXX=`which $TEST_CXX` 
cmake $ACCEL_DEFINES  ..
make
#cd -
