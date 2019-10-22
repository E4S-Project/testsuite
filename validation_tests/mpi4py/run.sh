#!/bin/bash
oneSpackHash(){ echo "/`spack find -l $1 | tail -n1 | awk '{print $1;}'`" ;  }
PYNUMPY=`oneSpackHash py-numpy`
. ../../setup.sh
spack activate py-mpi4py
spack activate $PYNUMPY
spack load $PYNUMPY
spack load py-mpi4py
spack load mpich
spack load python@3:
mpirun -np 4 python3 ./cpi.py
