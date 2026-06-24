#!/bin/bash
. ./setup.sh
set -x
#${TEST_RUN} 
${TEST_RUN_CMD} $TEST_RUN_PROCFLAG $MPIRANKS ./hypre_smoke_test 
 
