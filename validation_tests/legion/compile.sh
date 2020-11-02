#!/bin/bash -ex

./setup.sh

cd build
cmake CMakeLists.txt
make
cd ../

