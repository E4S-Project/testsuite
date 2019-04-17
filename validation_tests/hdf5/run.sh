#!/bin/bash
. ../../setup.sh
#spack load \trrej6a
spack load mpich
spack load hdf5+fortran

mpirun -np 8 ./ph5example # -v


