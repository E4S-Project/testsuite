#!/bin/bash

spack load blas
spack load lapack
spack load metis
spack load parmetis
spack load scalapack
spack load strumpack

mpicxx -fopenmp ./test_sparse_mpi.cpp -L/usr/local/packages/ecp/spack/opt/spack/linux-centos7-x86_64/gcc-7.3.0/strumpack-3.1.1-q4wwcyff7lzrrbwc6np5jxezv6iix7ig/lib -lgfortran -lmpifort -lmetis -lopenblas -lstrumpack -lscalapack
