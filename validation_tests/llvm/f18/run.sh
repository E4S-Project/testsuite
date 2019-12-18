#!/bin/bash

. ./setup.sh

cd lapack && make blas_testing lapack_testing
cd ..
cd gfortran.dg/build && ctest -j -R CMP && ctest -j -R EXE
cd ..

