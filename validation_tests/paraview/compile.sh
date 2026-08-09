#!/bin/bash

set -e

#----------------------------------------
# Variables for use later
#----------------------------------------
readonly workdir="build"
paraview_version="$( spack find --format '{version}' "/${PARAVIEW_HASH}" )"
readonly paraview_version
paraview_variants="$( spack find --format '{variants}' "/${PARAVIEW_HASH}" )"
readonly paraview_variants

paraview_has_variant () {
    local query="$1"
    readonly query

    shift

    echo "$paraview_variants" | \
        grep -q -e "+${query}\>"
}

if paraview_has_variant "shared"; then
    paraview_is_shared=true
else
    paraview_is_shared=false
fi
readonly paraview_is_shared

#----------------------------------------
# ParaView upstream examples
#----------------------------------------

# First clone ParaView and run its examples.
mkdir -p "$workdir/paraview-examples" # Must be removed in `clean.sh`
pushd "$workdir/paraview-examples"
# We do not need submodules. Also check out the version of ParaView the package
# has to ensure that the examples agree.
git clone --depth 1 -b "v${paraview_version}" https://gitlab.kitware.com/paraview/paraview.git src

paraview_examples_src="$( pwd )/src"
readonly paraview_examples_src

readonly paraview_examples_args=(
    "-DCMAKE_PREFIX_PATH=$paraview_ROOT"
    "-DBUILD_SHARED_LIBS=$paraview_is_shared"
    "$paraview_examples_src/Examples"
)

# Build with Ninja
mkdir build-ninja
pushd build-ninja
cmake -G Ninja "${paraview_examples_args[@]}"
cmake --build .
popd

# Build with Makefiles
mkdir build-make
pushd build-make
cmake -G "Unix Makefiles" "${paraview_examples_args[@]}"
cmake --build .
popd
popd
