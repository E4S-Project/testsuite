#!/bin/bash -e
set -x

. ./setup.sh

THISDIR=$(basename "$PWD")

python3 ./rdkit-cpu.py
