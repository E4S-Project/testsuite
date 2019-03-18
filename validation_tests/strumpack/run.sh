#!/bin/bash

spack load blas
spack load lapack
spack load metis
spack load parmetis
spack load scalapack
spack load strumpack

if [ ! -d "cbuckle" ]; then
	wget https://www.cise.ufl.edu/research/sparse/MM/TKK/cbuckle.tar.gz
	tar -xvzf cbuckle.tar.gz
	rm cbuckle.tar.gz
fi

mpirun -np 4 ./a.out m ./cbuckle/cbuckle.mtx



