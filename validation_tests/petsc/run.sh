#!/bin/bash
. ./setup.sh
set -x
cd petsc/build
eval $TEST_RUN_CMD $TEST_RUN_PROCFLAG 2 ./ex19 -da_refine 3 -snes_monitor_short -pc_type hypre

if [ -n "$CUDATEST" ]; then
	export PETSC_OPTIONS="-use_gpu_aware_mpi 0"
	eval $TEST_RUN_CMD $TEST_RUN_PROCFLAG 2 ./ex19	-snes_monitor -dm_mat_type mpiaijcusparse -dm_vec_type mpicuda -pc_type gamg -ksp_monitor -mg_levels_ksp_max_it 1
	#-dm_vec_type cuda -dm_mat_type aijcusparse -pc_type none -ksp_type fgmres -snes_monitor_short -snes_rtol 1.e-5
fi

if [ -n "$ROCMTEST" ]; then
	export PETSC_OPTIONS="-use_gpu_aware_mpi 0"
	eval $TEST_RUN_CMD $TEST_RUN_PROCFLAG 2 ./ex19 -snes_monitor -dm_mat_type mpiaijhipsparse -dm_vec_type mpihip -pc_type gamg -ksp_monitor -mg_levels_ksp_max_it 1
fi
cd ..
