#!/bin/bash
. ./setup.sh
set -x
mkdir install/build
cd install/build
cmake ..
make
