#!/bin/bash
. ../../setup.sh

THISDIR=`basename "$PWD"`
if [  "$THISDIR" = "umpire-cuda" ];then
        spackLoadUnique umpire+cuda
elif [  "$THISDIR" = "umpire-rocm" ];then
        spackLoadUnique "umpire+rocm $TEST_ROCM_ARCH"
        #export ACCEL_DEFINES="-DCMAKE_CXX_COMPILER=hipcc -DCMAKE_C_COMPILER=hipc"
else
        spackLoadUnique umpire~cuda~rocm
fi

