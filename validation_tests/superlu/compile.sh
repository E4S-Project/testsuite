#!/bin/bash -ex
. ./setup.sh
#spack load superlu
#spack load openblas threads=none
#gcc 
${TEST_CC} -g ./c_sample.c  -I${SUPERLU_ROOT}/include/ -L${SUPERLU_ROOT}/lib64 -L${SUPERLU_ROOT}/lib -L${OPENBLAS_ROOT}/lib  -lsuperlu -lopenblas -lm -o c_sample
