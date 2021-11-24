#!/bin/bash -e

. ./setup.sh

cmake . \
  -DCMAKE_Fortran_COMPILER=$TEST_FTN_MPI 

make -j2