#!/bin/bash
. ../../setup.sh

SUPERLU=`spack location -i superlu`
OPENBLAS=`spack location -i openblas`
gcc -g ./c_sample.c  -I${SUPERLU}/include/ -L${SUPERLU}/lib64 -L${OPENBLAS}/lib  -lsuperlu -lopenblas -lm -o c_sample




