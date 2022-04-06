#!/bin/bash

. ./setup.sh

if [ $e4s_print_color = true -a  -n "$TERM" ];
then
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'

BRED='\033[1;31m'
BGREEN='\033[1;32m'
BBLUE='\033[1;34m'

NC='\033[0m'
fi

for PROG in initfinalize sendrecv broadcast onesided ; do
    echo -e "Running ${BBLUE}${PROG}${NC}"
    #mpiexec  -n 4 
    eval $TEST_RUN ./${PROG}
    RC=$?
    if [ $? != 0 ]; then
	echo -e "                                 ${BRED}[FAILED]${NC}"
    else
	echo -e "                                 ${BGREEN}[PASSED]${NC}"
    fi
done

