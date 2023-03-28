#!/bin/bash
#!/bin/bash
. ./setup.sh
set -x
cd ./mpi_cuda_mm
  echo "Running with TAU:" 
  export TAU_METRICS=time:PAPI_L1_DCM
#  mpirun -np 4 
  ${TEST_RUN} tau_exec -T cupti -ebs -monitoring ./add 
  retVal=$?
  if [ $retVal -ne 0 ] ; then
    exit $retVal
  fi
  
  #cd bin
  pprof -a
  echo "To view performance data, please use:"
  echo "cd bin"
  echo "pprof -a | more "
#fi
# To use TAU:
#mpirun -np 4 tau_exec -ebs ./lu.W.4
# pprof

