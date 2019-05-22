#!/bin/bash
. ../../setup.sh
spack load mfem
spack load mpich 
export MFEM_INSTALL_DIR=`spack location --install-dir mfem`

mpirun -np 4 ./ex10p --mesh ./beam-quad.mesh

