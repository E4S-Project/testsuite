#!/bin/bash
. ./setup.sh
OMP_NUM_THREADS=4 eval $TEST_RUN ./ie2d
