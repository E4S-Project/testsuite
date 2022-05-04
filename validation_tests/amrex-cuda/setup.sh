#!/bin/bash
. ../../setup.sh

THISDIR=`basename "$PWD"`
USECUDA=0
USEROCM=0

if [ "$THISDIR" = "amrex-cuda" ];then
    spackLoadUnique amrex+cuda  #cuda_arch=80
    USECUDA=1
elif [ "$THISDIR" = "amrex-rocm" ];then
    spackLoadUnique amrex+rocm
    USEROCM=1
else
    spackLoadUnique amrex~rocm~cuda
fi

if ! command -v cmake >/dev/null 2>&1 ; then
    spackLoadUnique cmake@3.22.2:
fi
