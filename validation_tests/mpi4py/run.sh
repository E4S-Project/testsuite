#!/bin/bash -e
#oneSpackHash(){ echo "/`spack find -l $1 | tail -n1 | awk '{print $1;}'`" ;  }
#PYNUMPY=`oneSpackHash py-numpy`
. ../../setup.sh
#spackLoadUnique py-numpy
spackLoadUnique py-mpi4py
#PYNUMPY=`spackLoadUnique py-numpy`
spack activate py-mpi4py
spack activate py-numpy 
set -x
#spack load $PYNUMPY
#spack load py-mpi4py
#spack load mpich
#spack load python@3:

#mpirun -np 4 
eval $TEST_RUN python3 ./cpi.py
