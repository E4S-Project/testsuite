#!/bin/bash
. ./setup.sh
set -x
#g++
eval $TEST_CXX -fopenmp -c blockjacobi.cpp -I${KOKKOS_ROOT}/include -I${KOKKOS_KERNELS_ROOT}/include
#g++  
eval $TEST_CXX -fopenmp blockjacobi.o -o blockjacobi -L${KOKKOS_LIB_PATH} -L${KOKKOS_KERNELS_LIB_PATH} -lkokkoskernels -lkokkoscore

