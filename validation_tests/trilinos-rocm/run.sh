#!/bin/bash -e
. ./setup.sh
set -x
cd ./build
export OMP_NUM_THREADS=4
${TEST_RUN} ./Zoltan
