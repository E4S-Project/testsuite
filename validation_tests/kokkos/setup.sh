#!/bin/bash
. ../../setup.sh
if ! command -v cmake >/dev/null 2>&1 ; then
  spackLoadUniqueNoX cmake
fi


THISDIR=`basename "$PWD"`
if [  "$THISDIR" = "kokkos-cuda" ];then
        spackLoadUnique "kokkos+cuda $TEST_CUDA_ARCH"
elif [  "$THISDIR" = "kokkos-rocm" ];then
        spackLoadUnique "kokkos+rocm $TEST_ROCM_ARCH"
	export ACCEL_DEFINES="-DCMAKE_CXX_COMPILER=hipcc"
elif [  "$THISDIR" = "kokkos-sycl" ];then
        spackLoadUnique "kokkos +sycl"
	export ACCEL_DEFINES="-DCMAKE_CXX_COMPILER=icpx -DKokkos_ENABLE_SYCL=ON" # -DOpenMP_CXX_LIB_NAMES=libiomp5  -DCMAKE_LIBRARY_PATH='$(llvm-config --libdir)'"
else
	spackLoadUnique kokkos~cuda~rocm~sycl
	export ACCEL_DEFINES="-DCMAKE_CXX_COMPILER=`which g++`"
fi
