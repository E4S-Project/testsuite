#!/bin/bash

. ./setup.sh
#mpirun -np 2 
timeout 2m ${TEST_RUN_CMD} ${TEST_RUN_PROCFLAG} 2 ./heatdis_fault 128 test.cfg
echo continuing from test-simulated fault
timeout 2m ${TEST_RUN_CMD} ${TEST_RUN_PROCFLAG} 2 ./heatdis_fault 128 test.cfg
