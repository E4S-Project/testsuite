#!/bin/bash
spack load mpich
spack load trilinos
spack load hdf5
spack load hypre
spack load parmetis
spack load hdf5
spack load metis
spack load openblas superlu
spack load zlib
spack load matio
spack load scalapack
spack load blas
spack load suite-sparse
spack load netcdf boost@1.66.0

mpirun -np 4 ./METISCoarsening

