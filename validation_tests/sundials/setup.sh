#!/bin/bash
. ../../setup.sh

TESTNAME=sundials
TESTVERSION=
export THISDIR=`basename "$PWD"`
USECUDA=0
USEROCM=0

if [  "$THISDIR" = "$TESTNAME-cuda" ];then
        spackLoadUnique "$TESTNAME+cuda $TEST_CUDA_ARCH $TESTVERSION"
        USECUDA=1
elif [  "$THISDIR" = "$TESTNAME-rocm" ];then
        spackLoadUnique "$TESTNAME+rocm $TEST_ROCM_ARCH $TESTVERSION"
        USEROCM=1
else
        spackLoadUnique $TESTNAME ~cuda~rocm $TESTVERSION
fi

spackLoadUniqueNoX cmake@3.22.1:
