#!/bin/bash
. ../../setup.sh
spack load blas
spack load lapack
spack load magma
spack load cuda

make c 
