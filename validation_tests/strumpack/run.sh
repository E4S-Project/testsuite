#!/bin/bash
. ../../setup.sh
spack load openblas threads=openmp
spack load metis
spack load parmetis
spack load netlib-scalapack
spack load strumpack
spack load mpich

#if [ ! -d "cbuckle" ]; then
#	curl -O https://www.cise.ufl.edu/research/sparse/MM/TKK/cbuckle.tar.gz
#	tar -xvzf cbuckle.tar.gz
#	rm cbuckle.tar.gz
#fi

mpirun -np 4 ./a.out m ./cbuckle/cbuckle.mtx



