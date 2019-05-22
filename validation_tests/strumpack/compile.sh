#!/bin/bash
. ../../setup.sh
spack load openblas
spack load metis
spack load parmetis
spack load netlib-scalapack
spack load strumpack
spack load mpich

mpicxx -fopenmp ./test_sparse_mpi.cpp -L/usr/local/packages/ecp/spack/opt/spack/linux-centos7-x86_64/gcc-7.3.0/strumpack-3.1.1-q4wwcyff7lzrrbwc6np5jxezv6iix7ig/lib -lgfortran -lmpifort -lmetis -lopenblas -lstrumpack -lscalapack
