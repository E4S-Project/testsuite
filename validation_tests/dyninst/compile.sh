#!/bin/bash -e
. ./setup.sh

# Cmake doesn't like having DYNINST_ROOT set
Dyninst_DIR=${DYNINST_ROOT}/lib/cmake/Dyninst
unset DYNINST_ROOT

cd xmas_tree/build
cmake .. -DDyninst_DIR=${Dyninst_DIR} -DCMAKE_BUILD_TYPE=Release
make VERBOSE=1
