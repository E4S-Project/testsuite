#!/bin/bash
spack load mpich
spack load tau

cd bin
mpirun -np 4 ./lu.W.4

if [ $? = 0 ]; then
  echo "Running with TAU:" 
  mpirun -np 4 tau_exec -ebs ./lu.W.4
  echo "To view performance data, please use:"
  echo "cd bin"
  echo "pprof -a | more "
fi
# To use TAU:
#mpirun -np 4 tau_exec -ebs ./lu.W.4
# pprof

