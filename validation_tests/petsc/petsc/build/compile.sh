#!/bin/bash

. ./setup.sh

cmake \
  -DCMAKE_C_COMPILER=mpicc \
  -DMPI_DIR=${MPICH_ROOT} \
  -DPETSC_DIR=${PETSC_ROOT} \
  -DPETSC_INCLUDE_DIR=${PETSC_ROOT}/include \
  -DPETSC_LIBRARY_DIR=${PETSC_ROOT}/lib \
  ..

make -j2
