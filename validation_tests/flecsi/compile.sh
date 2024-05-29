#!/bin/bash
. ./setup.sh
set -x
set -e
mkdir -p poisson/build
cd poisson/build
cmake  ..
make
