#!/bin/bash
. ./setup.sh
#mpirun -n 2 
eval $TEST_RUN_CMD $TEST_RUN_PROCARG 2 ./castle
