#!/bin/bash
spack load superlu
spack load openblas threads=none
gcc -g ./c_sample.c  -I${SUPERLU_ROOT}/include/ -L${SUPERLU_ROOT}/lib64 -L${OPENBLAS_ROOT}/lib  -lsuperlu -lopenblas -lm -o c_sample
