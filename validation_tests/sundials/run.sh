#!/bin/bash -ex
. ./setup.sh
mpirun -np 4 examples/nvector/parallel/test_nvector_mpi 64 4
