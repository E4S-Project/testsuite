#!/bin/bash
. ./setup.sh
#g++
eval $TEST_CXX -fopenmp -c blockjacobi.cpp -I${KOKKOS}/include -I${KOKKOSKERNELS}/include
#g++  
eval $TEST_CXX-fopenmp blockjacobi.o -o blockjacobi -L${KOKKOS}/lib -L${KOKKOSKERNELS}/lib -lkokkoskernels -lkokkoscore

