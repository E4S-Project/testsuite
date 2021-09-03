#!/bin/bash -e
. ./setup.sh
set -x

cd mpi-vector-add
eval $TEST_RUN ./make run


