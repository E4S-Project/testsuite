#!/bin/bash
. ../../setup.sh
THISDIR=`basename "$PWD"`
spackLoadUnique caliper+rocm  #$THISDIR
spackLoadUnique hip
export HIP_PATH=$HIP_ROOT
#export E4S_TEST_SOURCE=>>>SOURCEFILE.EXT<<<
#export E4S_TEST_FLAGS=>>>FLAGS<<<
#export E4S_TEST_LIBS=>>>LIBS<<<
