#!/bin/bash -e
set -x

. ./setup.sh

THISDIR=$(basename "$PWD")

python3 ./sglang-cpu-smoketest.py
