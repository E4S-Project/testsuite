#!/bin/bash
. ./setup.sh
set -x
cd ./mpi_cuda_mm
make clean
make
