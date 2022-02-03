#!/bin/bash  
. ./setup.sh

#if ! command -v cmake >/dev/null 2>&1 ; then
#  spackLoadUnique cmake
#fi
set -e
set -x

mkdir -p build
cd build
cmake ..
make
cd -
