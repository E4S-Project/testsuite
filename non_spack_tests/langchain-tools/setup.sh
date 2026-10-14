#!/bin/bash

THISDIR=$(basename "$PWD")

if ! python3 -c "import importlib.util, sys; sys.exit(0 if importlib.util.find_spec('langchain.tools') else 1)"; then
    echo "Langchain Tools not installed."
    export SPACK_LOAD_RESULT=215
else
    echo "Continuing to Langchain Tools test"
fi
