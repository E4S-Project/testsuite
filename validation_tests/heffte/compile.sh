#!/bin/bash 
#-ex
. ./setup.sh
spackLoadUnique cmake
mkdir -p build
cd build
make $HEFFTE_ROOT/share/heffte/examples
make
cd -
