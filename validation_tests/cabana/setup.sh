#!/bin/bash

. ../../setup.sh

spackLoadUniqueNoX cmake

THISDIR=`basename "$PWD"`
USECUDA=0
USEROCM=0
if [  "$THISDIR" = "cabana-cuda" ];then
        spackLoadUnique cabana+cuda
        USECUDA=1
elif [  "$THISDIR" = "cabana-rocm" ];then
        spackLoadUnique cabana+rocm
        USEROCM=1
else
        spackLoadUnique cabana~cuda~rocm
fi
