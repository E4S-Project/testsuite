#!/bin/bash
. ../../setup.sh
spack load \trrej6a
spack load mpich
module load hdf5

mpirun -np 8 ./ph5example # -v


