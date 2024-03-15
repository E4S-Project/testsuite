#!/bin/bash

. ../../setup.sh

if ! command -v cmake >/dev/null 2>&1 ; then
  spackLoadUniqueNoX cmake
fi
export ARBORX_CC=$TEST_CC
export ARBORX_CXX=$TEST_CXX
THISDIR=`basename "$PWD"`
if [  "$THISDIR" = "arborx-cuda" ];then
        spackLoadUnique arborx+cuda $TEST_CUDA_ARCH
elif [  "$THISDIR" = "arborx-rocm" ];then
        spackLoadUnique "arborx+rocm $TEST_ROCM_ARCH"
	export ARBORX_CC=amdclang
	export ARBORX_CXX=amdclang++
else
        spackLoadUnique arborx~cuda~rocm
fi
