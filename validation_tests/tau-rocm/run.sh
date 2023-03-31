#!/bin/bash
#!/bin/bash
. ./setup.sh
set -x
cd ./mpi_vecadd
  echo "Running with TAU:" 
  export TAU_METRICS=time:PAPI_L1_DCM
#  mpirun -np 4 
  ${TEST_RUN} tau_exec -T rocm -rocm ./vectoradd_hip.exe 
  #tau_exec -T cupti -ebs -monitoring ./add 
  retVal=$?
  if [ $retVal -ne 0 ] ; then
    exit $retVal
  fi
  cd MULTI__time 
  pprof -a -s
  echo "To view performance data, please use:"
  echo "pprof -a | more "

