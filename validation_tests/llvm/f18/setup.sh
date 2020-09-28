#!/bin/bash

module load llvm/master/git

export F77_FC=gfortran
export F18_FC=gfortran
ulimit -s unlimited

