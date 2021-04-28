#!/bin/bash -e
. ./setup.sh
set -x
#mpirun -np 4 
${TEST_RUN} examples/nvector/parallel/test_nvector_mpi 64 4
