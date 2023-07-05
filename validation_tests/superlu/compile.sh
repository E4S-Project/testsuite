#!/bin/bash -e
. ./setup.sh
set -x

#Detect external BLAS link line and use if present, otherwise default to openblas
if [ -z ${TEST_BLAS_LINK+x} ] 
then BLAS_LINK="-L${OPENBLAS_ROOT}/lib -lopenblas" 
else BLAS_LINK=${TEST_BLAS_LINK}
fi
SUPERLU_LIBS=`pkg-config --libs --static $SUPERLU_LIB_PATH/pkgconfig/superlu.pc`
#/soft/packaging/spack/e4s/22.11-2022.12.30.003.001/spack/opt/spack/linux-sles15-x86_64/oneapi-2022.12.30.003.001/superlu-5.3.0-yprjyfu2lme6tp3gvyntyssqnvnmbqwi/lib/pkgconfig/superlu.pc`
${TEST_CC} -g -O2 ./c_sample.c  -I${SUPERLU_ROOT}/include/ -L${SUPERLU_LIB_PATH}  -lsuperlu ${BLAS_LINK}  -lm -o c_sample
#${TEST_CC} -g -O2 ./c_sample.c  -I${SUPERLU_ROOT}/include/ ${SUPERLU_LIBS} -o c_sample
