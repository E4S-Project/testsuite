#!/bin/bash
. ../../setup.sh

export THISDIR=`basename "$PWD"`
if [  "$THISDIR" = "flecsi-cuda" ];then
        spackLoadUnique "flesci+cuda $TEST_CUDA_ARCH"
elif [  "$THISDIR" = "flecsi-rocm" ];then
        spackLoadUnique "flecsi+rocm $TEST_ROCM_ARCH"
else
        spackLoadUnique flecsi~cuda~rocm
fi


spackLoadUnique flecsi 
