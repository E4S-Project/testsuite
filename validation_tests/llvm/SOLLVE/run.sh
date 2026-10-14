#!/bin/bash

. ./setup.sh

echo -e "${BBLUE}[Basic sanity checks]${NC}"
./basic > /dev/null
RC=$?
echo -n "Basic 1 "
if [ $RC != 0 ]; then
    echo -e "                                 ${BRED}[FAILED]${NC}"
else
    echo -e "                                 ${BGREEN}[PASSED]${NC}"
fi

./basic2 > /dev/null
RC=$?
echo -n "Basic 2 "
if [ $RC != 0 ]; then
    echo -e "                                 ${BRED}[FAILED]${NC}"
else
    echo -e "                                 ${BGREEN}[PASSED]${NC}"
fi

./matmul > /dev/null
RC=$?
echo -n "Matmul "
if [ $RC != 0 ]; then
    echo -e "                                  ${BRED}[FAILED]${NC}"
else
    echo -e "                                  ${BGREEN}[PASSED]${NC}"
fi

cd pragmas
./run.sh
cd ..
