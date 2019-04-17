#!/bin/bash
. ../../setup.sh
spack load blas
spack load lapack
spack load magma
spack load cuda

./example_sparse    
./example_sparse_operator    
./example_v1    
./example_v2


