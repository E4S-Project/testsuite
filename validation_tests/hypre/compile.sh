#!/bin/bash -ex
. ./setup.sh
#spack load openblas threads=openmp
#spack load hypre+internal-superlu
#spack load mpich

make bigint 
