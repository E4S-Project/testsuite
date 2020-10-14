#!/bin/bash -ex
. ./setup.sh
cd examples/nvector/parallel/
make CC=$TEST_CC_MPI
#mpicc
