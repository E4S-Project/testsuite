#!/bin/bash -e 
. ./setup.sh

if ! command -v cmake >/dev/null 2>&1 ; then
  spackLoadUnique cmake
fi

set -x

mkdir -p build
cd build
cmake ..
make
cd -
