#!/bin/bash

. ../setup.sh

MYCC=clang
MYFC=flang

MYOPT="-g -Wall "
MYOPTF="-fno-underscoring -g -Wall "

#RED='\033[0;31m'
#GREEN='\033[0;32m'
#BLUE='\033[0;34m'

#BRED='\033[1;31m'
#BGREEN='\033[1;32m'
#BBLUE='\033[1;34m'

#NC='\033[0m'

# Compile dynamic libraries
echo -e "${BBLUE}Compile dynamic libraries${NC}"

$MYCC $MYOPT -shared -o libaddc.so libaddc.c
RC=$?
echo -n "C library"
if [ $RC != 0 ]; then
    echo -e "                                 ${BRED}[FAILED]${NC}"
else
    echo -e "                                 ${BGREEN}[PASSED]${NC}"
fi
$MYFC $MYOPTF -shared -o libaddf.so libaddf.f90
RC=$?
echo -n "F90 library"
if [ $RC != 0 ]; then
    echo -e "                               ${BRED}[FAILED]${NC}"
else
    echo -e "                               ${BGREEN}[PASSED]${NC}"
fi
$MYFC $MYOPT -shared -o libaddf2.so libaddf.f90
RC=$?
echo -n "F90 library 2"
if [ $RC != 0 ]; then
    echo -e "                             ${BRED}[FAILED]${NC}"
else
    echo -e "                             ${BGREEN}[PASSED]${NC}"
fi

# Link against the dynamic libraries
echo -e "${BBLUE}Link against the dynamic libraries${NC}"

$MYFC $MYOPTF -o f_and_f f_and_f.f90 -L. -laddf
RC=$?
echo -n "F and F"
if [ $RC != 0 ]; then
    echo -e "                                   ${BRED}[FAILED]${NC}"
else
    echo -e "                                   ${BGREEN}[PASSED]${NC}"
fi
$MYCC $MYOPT -o callfunctions_c callfunctions.c -L. -laddc -laddf
RC=$?
echo -n "C and F and C"
if [ $RC != 0 ]; then
    echo -e "                             ${BRED}[FAILED]${NC}"
else
    echo -e "                             ${BGREEN}[PASSED]${NC}"
fi
$MYFC $MYOPTF -o callfunctions_f callfunctions.f90 -L. -laddc -laddf
RC=$?
echo -n "F and F and C"
if [ $RC != 0 ]; then
    echo -e "                             ${BRED}[FAILED]${NC}"
else
    echo -e "                             ${BGREEN}[PASSED]${NC}"
fi
$MYCC $MYOPT -o callfunctions2_c callfunctions2.c -L. -laddc -laddf2
RC=$?
echo -n "C and C and F"
if [ $RC != 0 ]; then
    echo -e "                             ${BRED}[FAILED]${NC}"
else
    echo -e "                             ${BGREEN}[PASSED]${NC}"
fi

# Compile static libraries
echo -e "${BBLUE}Compile static libraries${NC}"

$MYCC $MYOPT  -c libaddc.c
RC=$?
echo -n "C library"
if [ $RC != 0 ]; then
    echo -e "                                 ${BRED}[FAILED]${NC}"
else
    echo -e "                                 ${BGREEN}[PASSED]${NC}"
fi
$MYFC $MYOPTF -c libaddf.f90
RC=$?
echo -n "f90 library"
if [ $RC != 0 ]; then
    echo -e "                               ${BRED}[FAILED]${NC}"
else
    echo -e "                               ${BGREEN}[PASSED]${NC}"
fi
$MYFC $MYOPT  -o libaddf2.o -c libaddf.f90
RC=$?
echo -n "f90 library 2"
if [ $RC != 0 ]; then
    echo -e "                             ${BRED}[FAILED]${NC}"
else
    echo -e "                             ${BGREEN}[PASSED]${NC}"
fi

ar cr libaddc.a libaddc.o
ar cr libaddf.a libaddf.o
ar cr libaddf2.a libaddf2.o

# Use them
echo -e "${BBLUE}Link against the static libraries${NC}"

$MYFC $MYOPTF -o f_and_f f_and_f.f90 libaddf.a 
RC=$?
echo -n "F and F"
if [ $RC != 0 ]; then
    echo -e "                                   ${BRED}[FAILED]${NC}"
else
    echo -e "                                   ${BGREEN}[PASSED]${NC}"
fi
$MYCC $MYOPT  -o callfunctions_c_static callfunctions.c libaddf.a libaddc.a
RC=$?
echo -n "C and F and C"
if [ $RC != 0 ]; then
    echo -e "                             ${BRED}[FAILED]${NC}"
else
    echo -e "                             ${BGREEN}[PASSED]${NC}"
fi
$MYFC $MYOPTF -o callfunctions_f_static callfunctions.f90 libaddf.a libaddc.a
RC=$?
echo -n "F and F and C"
if [ $RC != 0 ]; then
    echo -e "                             ${BRED}[FAILED]${NC}"
else
    echo -e "                             ${BGREEN}[PASSED]${NC}"
fi
$MYCC $MYOPT  -o callfunctions2_c_static callfunctions2.c libaddf2.a libaddc.a
RC=$?
echo -n "C and C and F"
if [ $RC != 0 ]; then
    echo -e "                             ${BRED}[FAILED]${NC}"
else
    echo -e "                             ${BGREEN}[PASSED]${NC}"
fi

