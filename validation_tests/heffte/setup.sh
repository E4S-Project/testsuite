#!/bin/bash -e

. ../../setup.sh

TESTNAME=heffte
TESTVERSION=@2.2.0:
export THISDIR=`basename "$PWD"`
if [  "$THISDIR" = "$TESTNAME-cuda" ];then
        spackLoadUnique "$TESTNAME+cuda $TEST_CUDA_ARCH $TESTVERSION"
elif [  "$THISDIR" = "$TESTNAME-rocm" ];then
        spackLoadUnique "$TESTNAME+rocm $TEST_ROCM_ARCH $TESTVERSION"
elif [  "$THISDIR" = "$TESTNAME-fftw" ];then
	spackLoadUnique "$TESTNAME+fftw $TESTVERSION"
else
        spackLoadUnique $TESTNAME ~cuda~rocm~fftw $TESTVERSION
fi

