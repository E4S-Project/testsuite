#!/bin/bash

. ../../setup.sh

if ! command -v pkg-config >/dev/null 2>&1 ; then
  spackLoadUnique pkg-config
fi

TESTNAME=gasnet
TESTVERSION=
export THISDIR=`basename "$PWD"`
if [  "$THISDIR" = "$TESTNAME-cuda" ];then
        spackLoadUnique "$TESTNAME+cuda $TEST_CUDA_ARCH $TESTVERSION"
elif [  "$THISDIR" = "$TESTNAME-rocm" ];then
        spackLoadUnique "$TESTNAME+rocm $TEST_ROCM_ARCH $TESTVERSION"
else
        spackLoadUnique $TESTNAME ~cuda~rocm $TESTVERSION
fi

export PKG_CONFIG_PATH=$GASNET_ROOT/lib/pkgconfig/:$PKG_CONFIG_PATH
NP=4
