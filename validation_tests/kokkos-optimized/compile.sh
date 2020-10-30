#!/bin/bash -e 
. ./setup.sh
spackLoadUnique cmake
set -x

mkdir -p build
cd build
#echo $KOKKOS_HASH
#spack build-env $KOKKOS_HASH -- cmake ..
cmake ..
make
cd -
