#!/bin/bash
. ../../setup.sh

THISDIR=`basename "$PWD"`
if [  "$THISDIR" = "hypre-cuda" ];then
        spackLoadUnique "hypre@2.27.0:+cuda $TEST_CUDA_ARCH"
elif [  "$THISDIR" = "hypre-rocm" ];then
        spackLoadUnique "hypre@2.27.0:+rocm $TEST_ROCM_ARCH"
else
        spackLoadUnique hypre@2.27.0:~cuda~rocm
fi
