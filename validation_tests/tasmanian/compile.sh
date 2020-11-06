#!/bin/bash -e 
#-ex
. ./setup.sh
spackLoadUnique cmake
set -x
mkdir -p build
cd build
cmake $TASMANIAN_ROOT/share/Tasmanian/examples
make
cd -
