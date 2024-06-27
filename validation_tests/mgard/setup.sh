#!/bin/bash
. ../../setup.sh
export THISDIR=`basename "$PWD"`

TESTNAME=mgard
TESTVERSION=@2.10:
if [  "$THISDIR" = "$TESTNAME-cuda" ];then
        spackLoadUnique "$TESTNAME+cuda $TEST_CUDA_ARCH $TESTVERSION"
else
        spackLoadUnique $TESTNAME~cuda $TESTVERSION
fi

spack load pkgconf
