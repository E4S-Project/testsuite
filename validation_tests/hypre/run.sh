#!/bin/bash
. ../../setup.sh
spack load hypre+internal-superlu
spack load mpich

mpirun -np 8 ./ex5big
mpirun -np 8 ./ex15big



