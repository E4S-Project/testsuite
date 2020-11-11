#!/bin/bash -e
. ./setup.sh
set -x
mkdir -p build
cd build
cp  $SCR_ROOT/share/scr/examples/* .
make  

cd -
