#!/bin/bash
. ../../setup.sh
#spack load \trrej6a
spack load mpich
spack load hdf5+fortran

h5pcc -o ph5example ./ph5example.c


