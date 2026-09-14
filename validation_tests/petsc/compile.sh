#!/bin/bash
. ./setup.sh
set -x
set -e

# build ex19
mkdir -p build
cd build
cmake ..
#-DAXOM_DIR=$AXOM_ROOT -Dumpire_DIR=$UMPIRE_ROOT/lib/cmake/umpire ..
make
cd ..

# builds the e4s-cloud-examples petsc-cpu ex50 if cpu only test
if [ -z "$CUDATEST" ] && [ -z "$ROCMTEST" ]; then
        make ex50
fi

# builds the e4s-cloud-examples petsc-cuda bench_kspsolve if cuda test
if [ -n "$CUDATEST" ]; then
        make bench_kspsolve
fi
