#!/bin/bash -e
. ./setup.sh
set -x
for testdir in `readlink -f */`
do
    cd ${testdir}
    
    APP=`basename ${testdir}`
    ARG=""
    if [ "$APP" = "nine-pt-stencil-solver" -o "$APP" = "poisson-solver" -o "$APP" = "three-pt-stencil-solver" ]; then
	ARG="4"
    fi
    ./${APP} ${ARG}
done




