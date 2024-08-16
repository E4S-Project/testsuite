#!/bin/bash
. ../../setup.sh
THISDIR=`basename "$PWD"`
spackLoadUnique $THISDIR
#set -x
export HDF5_DIR=$HDF5_ROOT

export ABT_DIR=$ARGOBOTS_ROOT

export ASYNC_DIR=$HDF5_VOL_ASYNC_ROOT

