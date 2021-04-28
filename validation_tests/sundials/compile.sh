#!/bin/bash -e
. ./setup.sh
set -x
cd examples/nvector/parallel/
make CC=$TEST_CC_MPI
#mpicc
