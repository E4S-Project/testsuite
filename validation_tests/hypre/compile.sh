#!/bin/bash -e
. ./setup.sh
set -x
#spack load openblas threads=openmp
#spack load hypre+internal-superlu
#spack load mpich

make bigint 
