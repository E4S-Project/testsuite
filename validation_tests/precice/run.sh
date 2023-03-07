#!/bin/bash
. ./setup.sh
set -e
timeout -k 2m 5m ${TEST_RUN_SEQ} ./solverdummy precice-config.xml SolverOne MeshOne &
pid=$!
timeout -k 2m 5m ${TEST_RUN_SEQ} ./solverdummy precice-config.xml SolverTwo MeshTwo

wait $pid
