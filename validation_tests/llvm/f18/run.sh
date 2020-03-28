#!/bin/bash

. ./setup.sh

echo -e "\033[01;34m[Backend: gfortran]\e[0m"
export F18_FC=gfortran

cd lapack && make blas_testing lapack_testing TIMER=INT_ETIME
cd ..
cd gfortran.dg/build && ctest -j -R CMP && ctest -j -R EXE
cd ..

echo -e "\033[01;34m[Backend: clang]\e[0m"
export F18_FC=clang

cd lapack && make clean && make blas_testing lapack_testing
cd ..
cd gfortran.dg/build && ctest -j -R CMP && ctest -j -R EXE  TIMER=INT_CPU_TIME
cd ..

