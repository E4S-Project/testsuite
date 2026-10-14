#!/bin/bash -e
set -x

. ./setup.sh

THISDIR=$(basename "$PWD")

python3 ./langchain-tools-cpu.py
