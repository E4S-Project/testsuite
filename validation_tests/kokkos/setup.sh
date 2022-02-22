#!/bin/bash
. ../../setup.sh
if ! command -v cmake >/dev/null 2>&1 ; then
  spackLoadUnique cmake
fi


THISDIR=`basename "$PWD"`
if [  "$THISDIR" = "kokkos-cuda" ];then
        spackLoadUnique "kokkos+cuda $TEST_CUDA_ARCH"
elif [  "$THISDIR" = "kokkos-rocm" ];then
        spackLoadUnique "kokkos+rocm $TEST_ROCM_ARCH"
	export ACCEL_DEFINES="-DCMAKE_CXX_COMPILER=hipcc -DCMAKE_C_COMPILER=hipc"
elif [  "$THISDIR" = "kokkos-sycl" ];then
        spackLoadUnique "kokkos +sycl"
	export ACCEL_DEFINES="-DCMAKE_CXX_COMPILER=dpcpp"
else
	spackLoadUnique kokkos~cuda~rocm~sycl
fi
