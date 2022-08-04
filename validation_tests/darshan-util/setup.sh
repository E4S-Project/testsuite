#!/bin/bash
. ../../setup.sh
THISDIR=`basename "$PWD"`
spackLoadUnique $THISDIR
set -x
export E4S_SPACK_TEST_HASH=`spackHashFromName $THISDIR` 
