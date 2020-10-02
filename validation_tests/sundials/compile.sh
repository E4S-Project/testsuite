#!/bin/bash -ex
. ./setup.sh
cd examples/nvector/parallel/
make CC=mpicc
