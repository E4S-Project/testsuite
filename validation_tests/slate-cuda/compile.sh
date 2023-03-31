#!/bin/bash

. ./setup.sh

set -x

#Detect external BLAS link line and use if present, otherwise default to openblas
if [ -z ${TEST_BLAS_LINK+x} ]
then BLAS_LINK="-L${OPENBLAS_ROOT}/lib -lopenblas"
else BLAS_LINK=${TEST_BLAS_LINK}
fi

#Detect external CUDA link line and use if present, otherwise default 
if [ -z ${TEST_CUDA_LINK+x} ]
then CUDA_LINK="-L${CUDA_LIB_PATH} -lcudart -lcublas"
else CUDA_LINK=${TEST_CUDA_LINK}
fi


${TEST_CXX_MPI} -o slate04_blas slate04_blas.cc -I${BLASPP_ROOT}/include -I${CUDA_ROOT}/include  -I${LAPACKPP_ROOT}/include  -I${SLATE_ROOT}/include -L${BLASPP_LIB_PATH} ${CUDA_LINK}  -L${SLATE_LIB_PATH}  -lblaspp -lslate  ${BLAS_LINK} -fopenmp
