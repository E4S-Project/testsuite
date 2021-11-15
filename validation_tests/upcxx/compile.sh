#!/bin/bash -e
. ./setup.sh
set -x
#spack load openblas threads=openmp
#spack load hypre+internal-superlu
#spack load mpich

cp -r $UPCXX_ROOT/example .
cp $UPCXX_ROOT/share/doc/upcxx/LICENSE.txt example

cd example/prog-guide

make all 
