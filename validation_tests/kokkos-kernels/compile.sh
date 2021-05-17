#!/bin/bash
. ./setup.sh
set -x
#g++
eval $TEST_CXX -fopenmp -c blockjacobi.cpp -I${KOKKOS_ROOT}/include -I${KOKKOS_KERNELS_ROOT}/include
#g++  
eval $TEST_CXX -fopenmp blockjacobi.o -o blockjacobi -L${KOKKOS_ROOT}/lib -L${KOKKOS_KERNELS_ROOT}/lib -lkokkoskernels -lkokkoscore

