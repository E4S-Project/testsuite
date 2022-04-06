#!/bin/bash

#RED='\033[0;31m'
#GREEN='\033[0;32m'
#BLUE='\033[0;34m'

#BRED='\033[1;31m'
#BGREEN='\033[1;32m'
#BBLUE='\033[1;34m'

#NC='\033[0m'

OUTFILE="toto"

echo -e "${BBLUE}Generate code${NC}"

chill matmul.py &> $OUTFILE
RC=$?
echo -n "Matmul"
if [ $RC != 0 ]; then
    echo -e "                                   ${BRED}[FAILED]${NC}"
else
    echo -e "                                   ${BGREEN}[PASSED]${NC}"
fi
chill applyR.py &> $OUTFILE
RC=$?
echo -n "Apply R"
if [ $RC != 0 ]; then
    echo -e "                                  ${BRED}[FAILED]${NC}"
else
    echo -e "                                  ${BGREEN}[PASSED]${NC}"
fi
chill applyQ.py &> $OUTFILE
RC=$?
echo -n "Apply Q"
if [ $RC != 0 ]; then
    echo -e "                                  ${BRED}[FAILED]${NC}"
else
    echo -e "                                  ${BGREEN}[PASSED]${NC}"
fi

echo -e "${BBLUE}Compile the generated code${NC}"

clang -O3 -Wall -o hh src/hh_main.c src/matmul.c src/applyQ.c src/applyR.c src/householder2.c -lm
RC=$?
echo -n "Compiled original code"
if [ $RC != 0 ]; then
    echo -e "                   ${BRED}[FAILED]${NC}"
else
    echo -e "                   ${BGREEN}[PASSED]${NC}"
fi

clang -O3 -Wall -o hh_modified src/hh_main.c src/matmul_modified.c src/applyQ_modified.c src/applyR_modified.c src/householder2.c -lm
RC=$?
echo -n "Compiled modified code"
if [ $RC != 0 ]; then
    echo -e "                   ${BRED}[FAILED]${NC}"
else
    echo -e "                   ${BGREEN}[PASSED]${NC}"
fi

./hh 256 256 | grep -v flops > tmp
./hh_modified 256 256  | grep -v flops > tmp2
TOTO=`diff tmp tmp2`
echo -n "Produced code gives the same result:   "
if [ -z $TOTO ]; then
    echo -e "  ${BGREEN}[PASSED]${NC}"
else
    echo -e "  ${BRED}[FAILED]${NC}"
fi
rm tmp tmp2
