#!/bin/bash

. ./setup.sh

mpicxx -o slate04_blas slate04_blas.cc  -DSLATE_NO_CUDA -D__HIP_PLATFORM_AMD__  -I${BLASPP_ROOT}/include -I${LAPACKPP_ROOT}/include  -I${ROCBLAS_ROOT}/include  -I${HIP_ROOT}/include  -I${SLATE_ROOT}/include   -lslate -L${SLATE_LIB_PATH}  -lblaspp -L${BLASPP_LIB_PATH}  -fopenmp

