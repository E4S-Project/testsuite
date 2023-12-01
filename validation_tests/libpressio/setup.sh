#!/bin/bash
. ../../setup.sh
THISDIR=`basename "$PWD"`
spackLoadUnique cmake
spackLoadUnique $THISDIR
export E4S_SPACK_TEST_HASH=`spackHashFromName $THISDIR`

