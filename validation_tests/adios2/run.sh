#!/bin/bash
. ./setup.sh
set -x
set -e
cd build
ctest --output-on-failure
