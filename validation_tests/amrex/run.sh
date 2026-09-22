#!/bin/bash -e
. ./setup.sh
set -x

# Use global TEST_RUN launcher if set, otherwise fallback
RUN_CMD="${TEST_RUN:-${TEST_RUN_CMD:-mpirun} ${TEST_RUN_PROCFLAG:--np} ${TEST_RUN_PROCARG:-8}}"

${RUN_CMD} ./build/amrex_test amrex.arena_init_size=0
