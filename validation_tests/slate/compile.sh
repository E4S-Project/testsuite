#!/bin/bash

. ./setup.sh

mpicxx -o slate04_blas slate04_blas.cc -I${BLASPP_ROOT}/include -I${CUDA_ROOT}/include  -I${LAPACKPP_ROOT}/include  -I${SLATE_ROOT}/include -L${BLASPP_LIB_PATH} -L${CUDA_LIB_PATH}  -L${SLATE_LIB_PATH} -L${OPENBLAS_LIB_PATH} -lblaspp -lslate -lcudart -lopenblas -lcublas -fopenmp

