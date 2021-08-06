#!/bin/bash

. ./setup.sh
#mpirun -np 2 
${TEST_RUN_CMD} ${TEST_RUN_PROCFLAG} 2 ./heatdis_mem 128 test.cfg
