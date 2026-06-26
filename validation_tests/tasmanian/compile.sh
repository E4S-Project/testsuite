#!/bin/bash -e

. ./setup.sh

mkdir -p build

cd build

cp $TASMANIAN_ROOT/share/Tasmanian/testing/CMakeLists.txt . 

cmake .

make
