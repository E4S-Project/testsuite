#!/bin/bash
. ../../setup.sh
THISDIR=`basename "$PWD"`
spackLoadUnique $THISDIR
pythonHash=`spack find -dl /$LBANN_HASH | grep python | head -c 7`
spack load /$pythonHash
spack load --first py-numpy@1.26.4:

