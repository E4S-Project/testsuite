#!/bin/bash -e
. ./setup.sh
set -x
#mpirun -n 2 
#eval $TEST_RUN_CMD $TEST_RUN_PROCFLAG 2 
${TEST_RUN_SEQ} ./fieldOnSquare
