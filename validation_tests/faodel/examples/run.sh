#!/bin/bash
if [ $e4s_print_color = true -a  -n "$TERM" ];
then
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'

BRED='\033[1;31m'
BGREEN='\033[1;32m'
BBLUE='\033[1;34m'
fi

NC='\033[0m'

OUTFILE="/dev/null"

common/bootstrap/bootstrap_example 2>&1 > $OUTFILE
RC=$?
echo -n "Bootstrap"
if [ $? != 0 ]; then
    echo -e "                                 ${BRED}[FAILED]${NC}"
else
    echo -e "                                 ${BGREEN}[PASSED]${NC}"
fi

common/data_types/data_types 2>&1 > $OUTFILE
RC=$?
echo -n "Data types"
if [ $? != 0 ]; then
    echo -e "                                ${BRED}[FAILED]${NC}"
else
    echo -e "                                ${BGREEN}[PASSED]${NC}"
fi

common/info_interface/info_interface 2>&1 > $OUTFILE
RC=$?
echo -n "Info interface"
if [ $? != 0 ]; then
    echo -e "                            ${BRED}[FAILED]${NC}"
else
    echo -e "                            ${BGREEN}[PASSED]${NC}"
fi

common/logging_interface/logging_interface 2>&1 > $OUTFILE
RC=$?
echo -n "Logging interface"
if [ $? != 0 ]; then
    echo -e "                         ${BRED}[FAILED]${NC}"
else
    echo -e "                         ${BGREEN}[PASSED]${NC}"
fi

LD_LIBRARY_PATH=$SPACK_LD_LIBRARY_PATH:common/singleton:$LD_LIBRARY_PATH  ./common/singleton/singleton_example 2>&1 > $OUTFILE
RC=$?
echo -n "Singleton"
if [ $? != 0 ]; then
    echo -e "                                 ${BRED}[FAILED]${NC}"
else
    echo -e "                                 ${BGREEN}[PASSED]${NC}"
fi

