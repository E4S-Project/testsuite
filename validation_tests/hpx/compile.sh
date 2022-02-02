#!/bin/bash -e
. ./setup.sh
#spackLoadUnique cmake@3.17:
set -x

mkdir -p build
cmake -B build -S .
cmake --build build
