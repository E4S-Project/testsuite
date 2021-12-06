#!/bin/bash
#!/bin/bash
. ./setup.sh
#if [ $retVal -ne 0 ] ; then
#  exit $retVal
#fi

cd bin
#mpirun -np 4 
${TEST_RUN} ./lu.W.4 
retVal=$?
if [ $retVal -ne 0 ] ; then
  exit $retVal
fi
 

#if [ $? = 0 ]; then
  echo "Running with TAU:" 
  export TAU_METRICS=time:PAPI_L1_DCM
#  mpirun -np 4 
  ${TEST_RUN} tau_exec -ebs ./lu.W.4
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

