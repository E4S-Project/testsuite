#!/bin/bash

THISDIR=$(basename "$PWD")

if [ "$THISDIR" = "vllm-cuda" ] && ! { command -v nvidia-smi >/dev/null && nvidia-smi -L 2>/dev/null | grep -q '^GPU'; }; then
    echo "No NVIDIA GPU."
    export SPACK_LOAD_RESULT=215
elif [ "$THISDIR" = "vllm-rocm" ] && ! { command -v amd-smi >/dev/null && amd-smi list 2>/dev/null | grep -q '^GPU'; }; then
    echo "No AMD GPU."
    export SPACK_LOAD_RESULT=215
elif ! python3 -c "import importlib.util, sys; sys.exit(0 if importlib.util.find_spec('vllm') else 1)"; then
    echo "vLLM not installed."
    export SPACK_LOAD_RESULT=215
else
    echo "Continuing to vLLM test"
fi
