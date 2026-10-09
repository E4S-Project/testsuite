#!/bin/bash -e
set -x

THISDIR=$(basename "$PWD")

if [ "$THISDIR" = "tensorflow-cuda" ]; then
    python3 ./tensorflow-cuda-smoketest.py
elif [ "$THISDIR" = "tensorflow-rocm" ]; then
    python3 ./tensorflow-rocm-smoketest.py
else
    python3 ./f3.py
fi
