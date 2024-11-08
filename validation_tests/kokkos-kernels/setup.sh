#!/bin/bash
. ../../setup.sh

TESTNAME=kokkos-kernels
TESTVERSION=
export THISDIR=`basename "$PWD"`
if [  "$THISDIR" = "$TESTNAME" ];then
        spackLoadUnique "$TESTNAME+openmp $TEST_CUDA_ARCH $TESTVERSION"
elif [  "$THISDIR" = "$TESTNAME-cuda" ];then
        spackLoadUnique "$TESTNAME+cuda $TEST_ROCM_ARCH $TESTVERSION"
fi
#else
#        spackLoadUnique $TESTNAME ~cuda~rocm $TESTVERSION
#fi
#spackLoadUniqueNoR kokkos+openmp
#spackLoadUniqueNoR kokkos-kernels+openmp
#kokkosHash=$(spackLoadUniqueNoR kokkos+openmp)
#kokkoskernelsHash=$(spackLoadUniqueNoR kokkos-kernels+openmp)
#export KOKKOS=KOKKOS_ROOT #`spack location -i ${kokkosHash}`
#export KOKKOSKERNELS=KOKKOS_KERNELS_ROOT #`spack location -i ${kokkoskernelsHash}`

