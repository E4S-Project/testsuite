#!/bin/bash
. ../../setup.sh
spack load mfem 
spack load mpich
export MFEM_INSTALL_DIR=`spack location --install-dir mfem`
make  
