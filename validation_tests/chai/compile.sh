#!/bin/bash
. ./setup.sh
set -x
set -e
mkdir -p build
cd build
cmake .. -DBLT_SOURCE_DIR=$BLT_ROOT -Dcamp_DIR=$CAMP_ROOT -Dumpire_DIR=$UMPIRE_ROOT -Dchai_DIR=$CHAI_ROOT
make
