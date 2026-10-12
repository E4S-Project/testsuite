#!/bin/bash

THISDIR=$(basename "$PWD")

if ! python3 -c "import importlib.util, sys; sys.exit(0 if importlib.util.find_spec('sglang') else 1)"; then
    echo "SGLang not installed."
    export SPACK_LOAD_RESULT=215
else
    echo "Continuing to SGLang test"
fi
