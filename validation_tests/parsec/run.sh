#!/bin/bash

. ./setup.sh

RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'

BRED='\033[1;31m'
BGREEN='\033[1;32m'
BBLUE='\033[1;34m'

NC='\033[0m'

ret=0
for PROGNAME in dtd_test_allreduce write_check; do
    echo -e "${BBLUE}### TESTING${NC} $PROGNAME"
    echo -e "${BBLUE}# $TEST_RUN_CMD $TEST_RUN_PROCFLAG $NP ./${PROGNAME}${NC}"
    eval $TEST_RUN_CMD $TEST_RUN_PROCFLAG $NP ./${PROGNAME}
    RC=$?
    if [ $RC != 0 ]; then
      echo -e "${BBLUE}#   ${BRED}[FAILED]${NC}${BBLUE}: $PROGNAME${NC}"
      ret=1
    else
      echo -e "${BBLUE}#   ${BGREEN}[PASSED]${NC}${BBLUE}: $PROGNAME${NC}"
    fi
done

exit $ret

