#!/bin/bash

. ./setup.sh

#RED='\033[0;31m'
#GREEN='\033[0;32m'
#BLUE='\033[0;34m'

#BRED='\033[1;31m'
#BGREEN='\033[1;32m'
#BBLUE='\033[1;34m'

#NC='\033[0m'

OUTFILE="toto"

echo -e "${BBLUE}Basic sanity checks (backend: tapir)${NC}"
./basic &> $OUTFILE
RC=$?
echo -n "Basic OpenMP"
if [ $RC != 0 ]; then
    echo -e "                               ${BRED}[FAILED]${NC}"
else
    echo -e "                               ${BGREEN}[PASSED]${NC}"
fi

./basic2 &> $OUTFILE
RC=$?
echo -n "Basic OpenMP 2"
if [ $RC != 0 ]; then
    echo -e "                             ${BRED}[FAILED]${NC}"
else
    echo -e "                             ${BGREEN}[PASSED]${NC}"
fi

rm $OUTFILE

echo -e "${BBLUE}Backends: does the compiler recognize the syntax${NC}"
cd compilation
./run.sh
cd ..
