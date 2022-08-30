#!/bin/bash
. ../../setup.sh
THISDIR=`basename "$PWD"`
spackLoadUnique $THISDIR
export E4S_SPACK_TEST_HASH=`spackHashFromName $THISDIR`

