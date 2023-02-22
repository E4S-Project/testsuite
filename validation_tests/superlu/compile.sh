#!/bin/bash -e
. ./setup.sh
set -x

#Detect external BLAS link line and use if present, otherwise default to openblas
if [ -z ${TEST_BLAS_LINK+x} ] 
then BLAS_LINK="-L${OPENBLAS_ROOT}/lib -lopenblas" 
else BLAS_LINK=${TEST_BLAS_LINK}
fi

${TEST_CC} -g -O2 ./c_sample.c  -I${SUPERLU_ROOT}/include/ -L${SUPERLU_ROOT}/lib64 -L${SUPERLU_ROOT}/lib ${BLAS_LINK} -lsuperlu -lm -o c_sample
