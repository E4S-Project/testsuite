#!/bin/bash

THISDIR=$(basename "$PWD")

if [ "$THISDIR" = "pytorch-cuda" ] && ! { command -v nvidia-smi >/dev/null && nvidia-smi -L 2>/dev/null | grep -q '^GPU'; }; then
    echo "No NVIDIA GPU."
    export SPACK_LOAD_RESULT=215
elif [ "$THISDIR" = "pytorch-rocm" ] && ! { command -v amd-smi >/dev/null && amd-smi list 2>/dev/null | grep -q '^GPU'; }; then
    echo "No AMD GPU."
    export SPACK_LOAD_RESULT=215
elif ! python3 -c "import importlib.util, sys; sys.exit(0 if importlib.util.find_spec('torch') else 1)"; then
    echo "Tensorflow not installed."
    export SPACK_LOAD_RESULT=215
else
    echo "Continuing to Pytorch test"
fi

