#!/bin/bash
. ../../setup.sh

THISDIR=`basename "$PWD"`
USECUDA=0
USEROCM=0
if [  "$THISDIR" = "chai-cuda" ];then
        spackLoadUnique chai+cuda
        USECUDA=1
elif [  "$THISDIR" = "chai-rocm" ];then
        spackLoadUnique chai+rocm
        USEROCM=1
else
        spackLoadUnique chai~cuda~rocm
fi
