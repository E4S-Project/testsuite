#!/bin/bash -e
set -x

. ./setup.sh

THISDIR=$(basename "$PWD")

python3 ./a2a-cpu.py
