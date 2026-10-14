#!/bin/bash

THISDIR=$(basename "$PWD")

if ! python3 -c "import importlib.util, sys; sys.exit(0 if importlib.util.find_spec('gradio') else 1)"; then
    echo "Gradio not installed."
    export SPACK_LOAD_RESULT=215
else
    echo "Continuing to Gradio test"
fi
