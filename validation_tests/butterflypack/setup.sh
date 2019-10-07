#!/bin/bash
spack load arpack-ng
spack load openblas threads=openmp
spack load netlib-scalapack
spack load netlib-lapack
spack load butterflypack
