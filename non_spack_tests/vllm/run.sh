#!/bin/bash -e
set -x

. ./setup.sh

THISDIR=$(basename "$PWD")

if [ "$THISDIR" = "vllm-cuda" ]; then
    python3 ./vllm-gpu-smoketest.py
elif [ "$THISDIR" = "vllm-rocm" ]; then
    python3 ./vllm-gpu-smoketest.py
else
    python3 ./vllm-cpu-smoketest.py
fi
