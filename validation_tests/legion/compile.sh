#!/bin/bash -ex

. ./setup.sh

cd build
cmake CMakeLists.txt -DLegion_DIR=${LEGION_ROOT}/share/Legion/cmake

make
cd ../

