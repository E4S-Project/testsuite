#!/bin/bash
. ../../setup.sh
spack load hypre+internal-superlu
spack load mpich

make bigint 
