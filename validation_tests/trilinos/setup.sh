#!/bin/bash
. ../../setup.sh
#oneSpackHash(){ echo "/`spack find -l $1 | tail -n1 | awk '{print $1;}'`" ;  }
spackLoadUnique trilinos
export TSPEC=`spackLoadUnique trilinos`
export TRILINOS=`spack location -i $TSPEC`
#spack load metis
#spack load parmetis
#spack load hdf5+fortran
#spack load `oneSpackHash netcdf`
#spack load netlib-scalapack
#spack load suite-sparse
#spack load `oneSpackHash matio`
#spack load $TSPEC
#spack load `oneSpackHash hypre`
#spackLoadUnique openblas threads=openmp
