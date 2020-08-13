#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'

BRED='\033[1;31m'
BGREEN='\033[1;32m'
BBLUE='\033[1;34m'

NC='\033[0m'

RET=0

echo -e "${BLUE}   Basic test${NC}    "

cd basic
./run.sh
if [ $? != 0 ]; then
    echo -e "                                 ${BRED}[FAILED]${NC}"
    RET=1
else
    echo -e "                                 ${BGREEN}[PASSED]${NC}"
fi
cd ..

echo -e "${BLUE}   Using a library${NC}"

cd library
./run.sh
if [ $? != 0 ]; then
    echo -e "                                 ${BRED}[FAILED]${NC}"
    RET=$(($RET+1))
else
    echo -e "                                 ${BGREEN}[PASSED]${NC}"
fi
cd ..

exit $RET
