#!/bin/bash
export KOKKOS=`spack location -i kokkos%gcc@7.3.0`
echo "KOKKOS=$KOKKOS"
make

