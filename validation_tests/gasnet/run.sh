#!/bin/bash

. ./setup.sh

RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'

BRED='\033[1;31m'
BGREEN='\033[1;32m'
BBLUE='\033[1;34m'

NC='\033[0m'

OUTFILE="/dev/null"

for PROGNAME in testbarrier testhello  testsmall  testthreads  testqueue; do
    echo -n $PROGNAME
    #mpiexec -n 
    eval $TEST_RUN_CMD $TEST_RUN_PROCFLAG $NP ./${PROGNAME}  2>&1 > $OUTFILE
    RC=$?
    if [ $? != 0 ]; then
	echo -e "                                 ${BRED}[FAILED]${NC}"
    else
	echo -e "                                 ${BGREEN}[PASSED]${NC}"
    fi
done


rm $OUTFILE
