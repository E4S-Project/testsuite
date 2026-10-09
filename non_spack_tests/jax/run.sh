#!/bin/bash -e
set -x

THISDIR=$(basename "$PWD")

if [ "$THISDIR" = "jax-cuda" ]; then
    python3 ./jax-gpu-smoketest.py
elif [ "$THISDIR" = "jax-rocm" ]; then
    python3 ./jax-gpu-smoketest.py
else
    python3 ./jax-simple-neural-net-w-torch.py
fi

