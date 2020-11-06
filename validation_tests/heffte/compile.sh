#!/bin/bash 
#-ex
. ./setup.sh
spackLoadUnique cmake
mkdir -p build
cd build
cmake $HEFFTE_ROOT/share/heffte/examples
make
cd -
