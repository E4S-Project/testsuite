#!/bin/bash

set -e

#----------------------------------------
# Variables for use later
#----------------------------------------
readonly workdir="build"

#----------------------------------------
# ParaView upstream examples
#----------------------------------------
ctest --output-on-failure --test-dir "$workdir/paraview-examples/build-ninja"
ctest --output-on-failure --test-dir "$workdir/paraview-examples/build-make"
