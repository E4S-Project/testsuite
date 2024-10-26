#!/bin/bash
. ../../setup.sh

TESTNAME=upcxx
TESTVERSION=
export THISDIR=`basename "$PWD"`
if [  "$THISDIR" = "$TESTNAME-cuda" ];then
        spackLoadUnique "$TESTNAME+cuda $TEST_CUDA_ARCH $TESTVERSION"
elif [  "$THISDIR" = "$TESTNAME-rocm" ];then
        spackLoadUnique "$TESTNAME+rocm $TEST_ROCM_ARCH $TESTVERSION"
else
        spackLoadUnique $TESTNAME ~cuda~rocm $TESTVERSION
fi



#+internal-superlu
#spackLoadUnique openblas threads=openmp

# Default to UPC++'s portable smp-conduit backend, unless one was specified.
# This might differ from the default conduit for the UPC++ install,
# which might be a distributed conduit with special spawning requirements.
# The goal is to test multi-process without spawning problems.
export UPCXX_NETWORK=${UPCXX_NETWORK:-smp}

# This directs udp-conduit (if in use) to default to spawn all processes locally:
export GASNET_SPAWNFN=${GASNET_SPAWNFN:-L}
