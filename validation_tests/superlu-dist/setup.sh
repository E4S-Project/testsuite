#!/bin/bash

oneSpackHash(){ echo "/`spack find -l $1 | tail -n1 | awk '{print $1;}'`" ;  }

spack load openblas threads=openmp
spack load parmetis
spack load metis
spack load `oneSpackHash superlu-dist`
