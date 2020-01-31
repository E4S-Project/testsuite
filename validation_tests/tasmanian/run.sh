#!/bin/bash -ex
set -e
. ./setup.sh

OMP_NUM_THREADS=4 ./dreamtest

OMP_NUM_THREADS=4 ./gridtest
