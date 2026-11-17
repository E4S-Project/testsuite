#!/bin/bash

set -x

export OMP_NUM_THREADS=2
time ${TEST_RUN} exciting_mpismp input.xml
grep "EXCITING.*stopped" INFO.OUT
