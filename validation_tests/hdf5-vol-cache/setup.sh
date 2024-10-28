#!/bin/bash
. ../../setup.sh
THISDIR=`basename "$PWD"`
spackLoadUnique $THISDIR
#set -x
export HDF5_DIR=$HDF5_ROOT

export HDF5_VOL_DIR=$HDF5_VOL_CACHE_ROOT

