#!/bin/bash
spack load openblas threads=openmp
spack load tasmanian@6.0 #+python
spack load mpich
