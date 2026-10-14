#!/bin/bash

THISDIR=$(basename "$PWD")

if ! python3 -c "import importlib.util, sys; sys.exit(0 if importlib.util.find_spec('a2a') else 1)"; then
    echo "A2A not installed."
    export SPACK_LOAD_RESULT=215
else
    echo "Continuing to A2A test"
fi
