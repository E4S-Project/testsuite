#!/bin/bash
. ../../setup.sh
set -e
spackLoadUnique tau+mpi+cuda
cp -r ${TAU_ROOT}/examples/gpu/cuda/mpi_cuda_mm .
