#!/bin/bash

. ../../setup.sh
spack load mpich
export KOKKOS=`spack location -i kokkos`
echo "KOKKOS=$KOKKOS"
make

