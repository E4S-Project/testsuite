#!/bin/bash
. ../../setup.sh
if ! command -v cmake >/dev/null 2>&1 ; then
  spackLoadUnique cmake
fi


THISDIR=`basename "$PWD"`
if [  "$THISDIR" = "kokkos-cuda" ];then
        spackLoadUnique kokkos+cuda
elif [  "$THISDIR" = "kokkos-rocm" ];then
        spackLoadUnique "kokkos+rocm $TEST_ROCM_ARCH"
	export ACCEL_DEFINES="-DCMAKE_CXX_COMPILER=hipcc -DCMAKE_C_COMPILER=hipc"
else
	spackLoadUnique kokkos~cuda~rocm
fi
