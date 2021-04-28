#!/bin/bash -e
. ./setup.sh
set -x
#spack load \trrej6a
#spack load mpich
#spack load hdf5+fortran

#mpirun -np 8 
eval $TEST_RUN ./ph5example # -v


