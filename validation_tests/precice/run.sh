#!/bin/bash
. ./setup.sh
./solverdummy precice-config.xml SolverOne MeshOne &
pid=$!
./solverdummy precice-config.xml SolverTwo MeshTwo

wait $pid
