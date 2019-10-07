#!/bin/bash
set -e
. ./setup.sh

OMP_NUM_THREADS=4 ./gridtest
OMP_NUM_THREADS=4 ./tasdream -test
