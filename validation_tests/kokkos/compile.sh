#!/bin/bash -e 
. ./setup.sh
spackLoadUnique cmake
set -x

mkdir -p build
cd build
cmake ..
make
cd -
