#!/bin/bash
. ../../setup.sh
spack load openblas threads=openmp
spack load metis
spack load parmetis
spack load netlib-scalapack
spack load strumpack
spack load mpich

mpicxx -fopenmp ./test_sparse_mpi.cpp -I${METIS_ROOT}/include -I${STRUMPACK_ROOT}/include -L${METIS_ROOT}/lib -L${PARMETIS_ROOT}/lib -L${NETLIB_SCALAPACK_ROOT}/lib -L${OPENBLAS_ROOT}/lib -L${STRUMPACK_ROOT}/lib -lgfortran -lmpifort -lmetis -lopenblas -lstrumpack -lscalapack
