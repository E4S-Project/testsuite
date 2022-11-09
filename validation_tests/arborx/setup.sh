#!/bin/bash

. ../../setup.sh

if ! command -v cmake >/dev/null 2>&1 ; then
  spackLoadUnique cmake
fi

THISDIR=`basename "$PWD"`
if [  "$THISDIR" = "arborx-cuda" ];then
        spackLoadUnique arborx+cuda $TEST_CUDA_ARCH
elif [  "$THISDIR" = "arborx-rocm" ];then
        spackLoadUnique "arborx+rocm $TEST_ROCM_ARCH"
else
        spackLoadUnique arborx~cuda~rocm
fi
