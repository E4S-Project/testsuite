#!/bin/bash
. ./setup.sh
set -x
set -e

# runs e4s-cloud-example petsc-cpu test
echo "EX 50"
if [ -z "$CUDATEST" ] && [ -z "$ROCMTEST" ]; then
        eval $TEST_RUN_CMD $TEST_RUN_PROCFLAG 4 ./ex50 -da_grid_x 120 -da_grid_y 120 -pc_type lu -pc_factor_mat_solver_type superlu_dist -ksp_monitor -ksp_view
fi

# runs e4s-cloud-example petsc-cuda
echo "BENCH_KSPSOLVE"
if [ -n "$CUDATEST" ]; then
        eval $TEST_RUN_CMD $TEST_RUN_PROCFLAG 4 ./bench_kspsolve -n 128 -its 1000 -matmult -mat_type aijcusparse -use_gpu_aware_mpi 0
        unset PETSC_OPTIONS
fi

# run ex19
echo "EX 19"
cd build
eval $TEST_RUN_CMD $TEST_RUN_PROCFLAG 2 ./ex19 -da_refine 3 -snes_monitor_short #-pc_type hypre
set +e
if [ -n "$CUDATEST" ]; then
	export PETSC_OPTIONS="-use_gpu_aware_mpi 0"
	set -e
	eval $TEST_RUN_CMD $TEST_RUN_PROCFLAG 2 ./ex19	-snes_monitor -dm_mat_type mpiaijcusparse -dm_vec_type mpicuda -pc_type gamg -ksp_monitor -mg_levels_ksp_max_it 1
	set +e
	#-dm_vec_type cuda -dm_mat_type aijcusparse -pc_type none -ksp_type fgmres -snes_monitor_short -snes_rtol 1.e-5
fi

if [ -n "$ROCMTEST" ]; then
	export PETSC_OPTIONS="-use_gpu_aware_mpi 0"
	set -e
	eval $TEST_RUN_CMD $TEST_RUN_PROCFLAG 2 ./ex19 -snes_monitor -dm_mat_type mpiaijhipsparse -dm_vec_type mpihip -pc_type gamg -ksp_monitor -mg_levels_ksp_max_it 1
	set +e
fi
cd ..
