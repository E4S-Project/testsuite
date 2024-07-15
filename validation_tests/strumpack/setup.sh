#!/bin/bash

. ../../setup.sh

if ! command -v cmake >/dev/null 2>&1 ; then
  spackLoadUniqueNoX cmake
fi

TESTNAME=strumpack
TESTVERSION=
export runArg=
export THISDIR=`basename "$PWD"`
if [  "$THISDIR" = "$TESTNAME-cuda" ];then
        spackLoadUnique "$TESTNAME+cuda $TEST_CUDA_ARCH $TESTVERSION"
elif [  "$THISDIR" = "$TESTNAME-rocm" ];then
        spackLoadUnique "$TESTNAME+rocm $TEST_ROCM_ARCH $TESTVERSION"
else
        spackLoadUnique $TESTNAME ~cuda~rocm $TESTVERSION
	export runArg="--sp_disable_gpu"
fi


