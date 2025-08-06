#!/bin/bash
. ../../setup.sh
THISDIR=`basename "$PWD"`

if [[ "$THISDIR" = *"-cuda" ]];then
	TESTNAME="${THISDIR%-cuda}"
        spackLoadUnique "$TESTNAME+cuda $TEST_CUDA_ARCH $TESTVERSION"
elif [[ "$THISDIR" = *"-rocm" ]];then
        TESTNAME="${THISDIR%-rocm}"
        spackLoadUnique "$TESTNAME+rocm $TEST_CUDA_ARCH $TESTVERSION"
else
        spackLoadUnique $THISDIR ~cuda~rocm $TESTVERSION
fi

export CEED_DIR=$LIBCEED_ROOT
#export E4S_TEST_SOURCE=>>>SOURCEFILE.EXT<<<
#export E4S_TEST_FLAGS=>>>FLAGS<<<
#export E4S_TEST_LIBS=>>>LIBS<<<
