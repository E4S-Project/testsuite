#!/bin/bash

. ../setup.sh

# Dynamic libraries

#RED='\033[0;31m'
#GREEN='\033[0;32m'
#BLUE='\033[0;34m'

#BRED='\033[1;31m'
#BGREEN='\033[1;32m'
#BBLUE='\033[1;34m'

#NC='\033[0m'

# TODO test it the exec runs

echo -e "${BBLUE}[Dynamic libraries]${NC}"

LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH ./callfunctions_c > tmpc
LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH ./callfunctions2_c > tmpc2
LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH ./callfunctions_f > tmpf

TOTO=`diff tmpc tmpf`
echo -n "Comparing C and Fortran calls:  "
if [ -z $TOTO ]; then
    echo -e "                                 ${BGREEN}[PASSED]${NC}"
else
    echo -e "                                 ${BRED}[FAILED]${NC}"    
fi
TOTO=`diff tmpc2 tmpc`
echo -n "Comparing C with and without _: "
if [ -z $TOTO ]; then
    echo -e "                                 ${BGREEN}[PASSED]${NC}"
else
    echo -e "                                 ${BRED}[FAILED]${NC}"    
fi
rm tmpc tmpc2 tmpf

# Static libraries

echo -e "${BBLUE}[Static libraries]${NC}"

./callfunctions_c_static > tmpc
./callfunctions2_c_static > tmpc2
./callfunctions_f_static> tmpf

TOTO=`diff tmpc tmpf`
echo -n "Comparing C and Fortran calls:  "
if [ -z $TOTO ]; then
    echo -e "                                 ${BGREEN}[PASSED]${NC}"
else
    echo -e "                                 ${BRED}[FAILED]${NC}"    
fi
TOTO=`diff tmpc2 tmpc`
echo -n "Comparing C with and without _: "
if [ -z $TOTO ]; then
    echo -e "                                 ${BGREEN}[PASSED]${NC}"
else
    echo -e "                                 ${BRED}[FAILED]${NC}"    
fi
rm tmpc tmpc2 tmpf

