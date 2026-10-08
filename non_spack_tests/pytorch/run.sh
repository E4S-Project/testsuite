#!/bin/bash -e
set -x

THISDIR=$(basename "$PWD")

if [ "$THISDIR" = "pytorch-cuda" ]; then
    python3 ./torch-simple-nn.py
elif [ "$THISDIR" = "pytorch-rocm" ]; then
    python3 ./torch-simple-nn.py
else
    python3 ./f4.py
fi
