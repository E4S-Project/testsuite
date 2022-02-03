#!/bin/bash
. ../../setup.sh
if ! command -v cmake >/dev/null 2>&1 ; then
  spackLoadUnique cmake
fi


THISDIR=`basename "$PWD"`
if [  "$THISDIR" = "kokkos-cuda" ];then
        spackLoadUnique kokkos+cuda
elif [  "$THISDIR" = "kokkos-rocm" ];then
        spackLoadUnique kokkos+rocm
else
	spackLoadUnique kokkos~cuda~rocm
fi
