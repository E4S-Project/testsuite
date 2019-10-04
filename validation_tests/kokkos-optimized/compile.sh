#!/bin/bash
. ./setup.sh
export KOKKOS=`spack location -i kokkos`
echo "KOKKOS=$KOKKOS"
make
