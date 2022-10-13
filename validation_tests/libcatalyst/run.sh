#!/bin/bash

set -e

#----------------------------------------
# Variables for use later
#----------------------------------------
readonly workdir="build"

#----------------------------------------
# Catalyst upstream examples
#----------------------------------------
ctest --output-on-failure --test-dir "$workdir/libcatalyst-examples/build-ninja"
ctest --output-on-failure --test-dir "$workdir/libcatalyst-examples/build-make"
