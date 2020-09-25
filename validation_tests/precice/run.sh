#!/bin/bash

$(./solverdummy precice-config.xml SolverOne MeshOne) &
pid=$!
./solverdummy precice-config.xml SolverTwo MeshTwo

wait $pid
