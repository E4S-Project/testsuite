#!/bin/bash -e
. ./setup.sh
set -x
cd ./build
ctest
