#!/bin/bash
export KOKKOS=`spack location -i kokkos`
make clean
rm -f *.o
