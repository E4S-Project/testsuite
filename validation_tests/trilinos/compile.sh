#!/bin/bash -e 
. ./setup.sh
set -x

mkdir -p build
cd build
cmake ..
make
cd -
