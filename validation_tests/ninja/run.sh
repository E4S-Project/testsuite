#!/bin/bash


RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'

BRED='\033[1;31m'
BGREEN='\033[1;32m'
BBLUE='\033[1;34m'

NC='\033[0m'


# Basic tests
echo -e "${BBLUE}Basic test${NC}"
cd basic
./run.sh
if [ $? != 0 ]; then
    echo -e "                                 ${BRED}[FAILED]${NC}"
else
    echo -e "                                 ${BGREEN}[PASSED]${NC}"
fi
cd ..

# Compile a library and link against it
echo -e "${BBLUE}Using a library${NC}"
cd library
./run.sh
if [ $? != 0 ]; then
    echo -e "                                 ${BRED}[FAILED]${NC}"
else
    echo -e "                                 ${BGREEN}[PASSED]${NC}"
fi
cd ..

# Use with CMake
echo -e "${BBLUE}Using a generator: CMake${NC}"
cd generators/cmake
./run.sh
if [ $? != 0 ]; then
    echo -e "                                 ${BRED}[FAILED]${NC}"
else
    echo -e "                                 ${BGREEN}[PASSED]${NC}"
fi
cd ../..

# ... and with Python
echo -e "${BBLUE}Using a generator: Python${NC}"
cd generators/python
./run.sh
if [ $? != 0 ]; then
    echo -e "                                 ${BRED}[FAILED]${NC}"
else
    echo -e "                                 ${BGREEN}[PASSED]${NC}"
fi
cd ../..
