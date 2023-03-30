#!/bin/bash -e
. ./setup.sh
set -x
for testdir in $GINKGO_DIRS  #`readlink -f */`
do
    cd ${testdir}
    
    APP=`basename ${testdir}`
    ARG=""
   # if [ "$APP" = "nine-pt-stencil-solver" -o "$APP" = "poisson-solver" -o "$APP" = "three-pt-stencil-solver" ]; then
#	ARG="4"
#    fi
    time timeout 2m srun ./${APP} ${ARG}
    cd ..
done




