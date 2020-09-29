#!/bin/bash

. ./setup.sh

RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'

BRED='\033[1;31m'
BGREEN='\033[1;32m'
BBLUE='\033[1;34m'

NC='\033[0m'

function start_chill(){
    
    # Verification: can we start CHiLL
    
    for EXEC in "chill" "cuda-chill" ; do
	echo "exit()" | $EXEC > /dev/null
	RET=$?
	echo -n "Start :" $EXEC "     "
	if [ $RET == 0 ] ; then
	    echo -e "                  ${BGREEN}[PASSED]${NC}"
	else
	    echo -e "                  ${BRED}[FAILED]${NC}"
	fi
    done    
}

echo -e "${BBLUE}Sanity check${NC}"
start_chill
