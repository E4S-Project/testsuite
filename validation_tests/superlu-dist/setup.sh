#!/bin/bash
. ../../setup.sh
#oneSpackHash(){ echo "/`spack find -l $1 | tail -n1 | awk '{print $1;}'`" ;  }

spackLoadUnique superlu-dist
spackLoadUnique openblas threads=openmp

#spack load openblas threads=openmp
#spack load parmetis
#spack load metis
#spack load `oneSpackHash superlu-dist`
