#!/bin/bash

. ./setup.sh

echo -e "${BBLUE}[Basic sanity checks]${NC}"
./basic > /dev/null
RC=$?
echo -n "Basic 1 "
if [ $? != 0 ]; then
    echo -e "                                 ${BRED}[FAILED]${NC}"
else
    echo -e "                                 ${BGREEN}[PASSED]${NC}"
fi

./basic2 > /dev/null
RC=$?
echo -n "Basic 2 "
if [ $? != 0 ]; then
    echo -e "                                 ${BRED}[FAILED]${NC}"
else
    echo -e "                                 ${BGREEN}[PASSED]${NC}"
fi

./matmul > /dev/null
RC=$?
echo -n "Matmul "
if [ $? != 0 ]; then
    echo -e "                                  ${BRED}[FAILED]${NC}"
else
    echo -e "                                  ${BGREEN}[PASSED]${NC}"
fi

cd pragmas
./run.sh
cd ..
