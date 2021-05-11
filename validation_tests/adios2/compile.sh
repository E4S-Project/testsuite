#!/bin/bash

. ./setup.sh
set -x
export ADIOS2_LIB_PATH=${ADIOS2_ROOT}/lib

if [[ ! -d $ADIOS2_LIB_PATH ]]; then
    export ADIOS2_LIB_PATH=${ADIOS2_ROOT}/lib64
fi

make
