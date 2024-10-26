#!/bin/bash
. ../../setup.sh
THISDIR=`basename "$PWD"`

TESTNAME=lammps
TESTVERSION=
export THISDIR=`basename "$PWD"`
if [  "$THISDIR" = "$TESTNAME-cuda" ];then
        spackLoadUnique "$TESTNAME+cuda $TEST_CUDA_ARCH $TESTVERSION"
	#export E4S_SPACK_TEST_HASH=`spackHashFromName $TESTNAME+cuda`
elif [  "$THISDIR" = "$TESTNAME-rocm" ];then
        spackLoadUnique "$TESTNAME+rocm $TEST_ROCM_ARCH $TESTVERSION"
	#export E4S_SPACK_TEST_HASH=`spackHashFromName $TESTNAME+rocm`
else
        spackLoadUnique $TESTNAME ~cuda~rocm $TESTVERSION
	#export E4S_SPACK_TEST_HASH=`spackHashFromName $TESTNAME`
fi

