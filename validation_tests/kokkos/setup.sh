#!/bin/bash
. ../../setup.sh
THISDIR=`basename "$PWD"`
if [  "$THISDIR" = "kokkos-cuda" ];then
        spackLoadUnique kokkos+cuda
elif [  "$THISDIR" = "kokkos-rocm" ];then
        spackLoadUnique kokkos+rocm
else
	spackLoadUnique kokkos~cuda~rocm
fi
