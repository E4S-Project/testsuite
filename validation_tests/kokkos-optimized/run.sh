#!/bin/bash -ex
. ./setup.sh
#ldd ./lulesh.host
#ulimit -c unlimited
export OMP_PROC_BIND=spread 
export OMP_PLACES=threads
#mpirun -np 8 
eval $TEST_RUN ./lulesh.host -i 4
result=$?
if [ $result = 0 ]; then
  echo "Running with TAU:" 
  #mpirun -np 8 
  eval $TEST_RUN tau_exec -ebs ./lulesh.host -i 4 
  result=$?
  echo "To view performance data, please use:"
  echo "pprof -a | more "
fi
exit $result

