#!/bin/bash

. ../../setup.sh

#mpirun -np 2 
eval $TEST_RUN_CMD $TEST_RUN_PROCFLAG 2 ./ex19 -da_refine 3 -snes_monitor_short -pc_type hypre
