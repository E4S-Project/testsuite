#!/bin/bash -e
. ./setup.sh
set -x

#Detect external BLAS link line and use if present, otherwise default to openblas
if [ -z ${TEST_BLAS_LINK+x} ]
then BLAS_LINK="-L${OPENBLAS_ROOT}/lib -lopenblas"
else BLAS_LINK=${TEST_BLAS_LINK}
fi


VERBOSE=1 make c 
