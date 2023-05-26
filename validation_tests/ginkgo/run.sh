#!/bin/bash -e
. ./setup.sh
set -x
for testdir in $GINKGO_DIRS  #`readlink -f */`
do
    cd ${testdir}
    
    APP=`basename ${testdir}`
    #ARG=""
   # if [ "$APP" = "nine-pt-stencil-solver" -o "$APP" = "poisson-solver" -o "$APP" = "three-pt-stencil-solver" ]; then
#	ARG="4"
#    fi

    if [ "$APP" = "minimal-cuda-solver" ]; then
    	cat data/A.mtx data/b.mtx data/x0.mtx | ./minimal-cuda-solver
    elif [ "$APP" = "simple-solver" -a "$THISDIR" = "ginkgo-cuda" ]; then
	./simple-solver cuda
    else 
    time timeout 2m ${TEST_RUN} ./${APP} ${ARG}
    fi
    cd ..
done




