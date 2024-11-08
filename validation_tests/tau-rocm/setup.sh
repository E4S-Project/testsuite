#!/bin/bash
. ../../setup.sh
set -e
spackLoadUnique tau+mpi+rocm 
cp -r ${TAU_ROOT}/examples/gpu/hip/mpi_vecadd/ .
