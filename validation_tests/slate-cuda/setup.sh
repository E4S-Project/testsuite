#!/bin/bash

. ../../setup.sh

TESTNAME=slate
TESTVERSION=
export THISDIR=`basename "$PWD"`
if [  "$THISDIR" = "$TESTNAME-cuda" ];then
        spackLoadUnique "$TESTNAME+cuda $TEST_CUDA_ARCH $TESTVERSION"
	export SLATE_CUDA_TEST=1
elif [  "$THISDIR" = "$TESTNAME-rocm" ];then
        spackLoadUnique "$TESTNAME+rocm $TEST_ROCM_ARCH $TESTVERSION"
	export SLATE_ROCM_TEST=1
else
        spackLoadUnique $TESTNAME ~cuda~rocm $TESTVERSION
	export SLATE_NOACC_TEST=1
fi

