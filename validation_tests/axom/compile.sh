#!/bin/bash
. ./setup.sh
set -x
set -e
mkdir -p build
cd build
cmake  -DAXOM_DIR=$AXOM_ROOT -Dumpire_DIR=$UMPIRE_ROOT/lib/cmake/umpire ..
make
