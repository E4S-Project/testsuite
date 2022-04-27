#!/bin/bash -e
. ./setup.sh
set -x

git clone https://github.com/AMReX-Codes/amrex-tutorials.git
mv amrex-tutorials/ExampleCodes/* .

mkdir -p build
cd build

cmake ..
make -j8
cd -
