#!/bin/bash

set -e

#----------------------------------------
# Variables for use later
#----------------------------------------
readonly workdir="build"

#----------------------------------------
# Catalyst upstream examples
#----------------------------------------

# First clone catalyst and run its examples.
mkdir -p "$workdir/libcatalyst-examples" # Must be removed in `clean.sh`
pushd "$workdir/libcatalyst-examples"
git clone https://gitlab.kitware.com/paraview/catalyst.git src

catalyst_examples_src="$( pwd )/src"
readonly catalyst_examples_src

readonly catalyst_examples_args=(
    "-DCMAKE_PREFIX_PATH=$catalyst_ROOT"
    -DCATALYST_BUILD_REPLAY=ON
    -DBUILD_SHARED_LIBS=ON
    "$catalyst_examples_src/examples"
)

# Build with Ninja
mkdir build-ninja
pushd build-ninja
cmake -G Ninja "${catalyst_examples_args[@]}"
cmake --build .
popd

# Build with Makefiles
mkdir build-make
pushd build-make
cmake -G "Unix Makefiles" "${catalyst_examples_args[@]}"
cmake --build .
popd
popd
