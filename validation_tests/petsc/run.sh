#!/bin/bash
. ./setup.sh
set -x
cd petsc/build
eval $TEST_RUN_CMD $TEST_RUN_PROCFLAG 2 ./ex19 -da_refine 3 -snes_monitor_short -pc_type hypre
#./run.sh
cd ..
