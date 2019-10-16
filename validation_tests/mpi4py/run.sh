#!/bin/bash
. ../../setup.sh
spack activate py-mpi4py
spack activate py-numpy
spack load py-numpy
spack load py-mpi4py
spack load mpich
spack load python@3:
mpirun -np 4 python3 ./cpi.py
