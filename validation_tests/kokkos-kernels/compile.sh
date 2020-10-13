#!/bin/bash
. ./setup.sh
g++ -fopenmp -c blockjacobi.cpp -I${KOKKOS}/include -I${KOKKOSKERNELS}/include
g++  -fopenmp blockjacobi.o -o blockjacobi -L${KOKKOS}/lib -L${KOKKOSKERNELS}/lib -lkokkoskernels -lkokkoscore

