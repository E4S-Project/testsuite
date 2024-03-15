#!/bin/bash -e
. ./setup.sh
#spackLoadUniqueNoX cmake@3.17:
set -x
export CXX=`which $TEST_CXX`
mkdir -p build
cmake -B build -S .
cmake --build build
