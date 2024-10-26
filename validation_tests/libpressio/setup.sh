#!/bin/bash
. ../../setup.sh
THISDIR=`basename "$PWD"`
spackLoadUniqueNoX cmake

TESTNAME=libpressio
TESTVERSION=
export THISDIR=`basename "$PWD"`
if [  "$THISDIR" = "$TESTNAME-cuda" ];then
        spackLoadUnique "$TESTNAME+cuda $TEST_CUDA_ARCH $TESTVERSION"
#elif [  "$THISDIR" = "$TESTNAME-rocm" ];then
#        spackLoadUnique "$TESTNAME+rocm $TEST_ROCM_ARCH $TESTVERSION"
else
        spackLoadUnique $TESTNAME ~cuda $TESTVERSION
fi


#spackLoadUnique $THISDIR
#export E4S_SPACK_TEST_HASH=`spackHashFromName $THISDIR`

