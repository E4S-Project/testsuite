#!/bin/bash

. ./setup.sh
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'

BRED='\033[1;31m'
BGREEN='\033[1;32m'
BBLUE='\033[1;34m'

NC='\033[0m'

echo -e "${BBLUE}Execution of OpenACC programs${NC}"

./parallel
RC=$?
echo -n "Parallel"
if [ $RC != 0 ]; then
    echo -e "                                 ${BRED}[FAILED]${NC}"
else
    echo -e "                                 ${BGREEN}[PASSED]${NC}"
fi
./jacobi
RC=$?
echo -n "Jacobi"
if [ $RC != 0 ]; then
    echo -e "                                 ${BRED}[FAILED]${NC}"
else
    echo -e "                                 ${BGREEN}[PASSED]${NC}"
fi
./jacobi_data
RC=$?
echo -n "Jacobi data"
if [ $RC != 0 ]; then
    echo -e "                           ${BRED}[FAILED]${NC}"
else
    echo -e "                           ${BGREEN}[PASSED]${NC}"
fi
./jacobi_data2
RC=$?
echo -n "Jacobi data 2"
if [ $RC != 0 ]; then
    echo -e "                         ${BRED}[FAILED]${NC}"
else
    echo -e "                         ${BGREEN}[PASSED]${NC}"
fi
./householder3 256 256
RC=$?
echo -n "Householder"
if [ $RC != 0 ]; then
    echo -e "                                 ${BRED}[FAILED]${NC}"
else
    echo -e "                                 ${BGREEN}[PASSED]${NC}"
fi

NB=4
STARTED=`./gang $NB | grep "toto" | wc -l`
echo -n "Gang:"
if [ $NB == $STARTED ]; then
    echo -e "                                 ${BGREEN}[PASSED]${NC}"
else
    echo -e "                                 ${BRED}[FAILED]${NC}", $STARTED, " instead of ", $NB, "requested"
fi


./inout
RC=$?
echo -n "Data in/out:"
if [ $RC == 0 ] ; then
    echo -e "                                 ${BGREEN}[PASSED]${NC}"
else
    echo -e "                                 ${BRED}[FAILED]${NC}"
fi

./inout_data
RC=$?
echo -n "Data in/out (2):"
if [ $? == 0 ] ; then
    echo -e "                                 ${BGREEN}[PASSED]${NC}"
else
    echo -e "                                 ${BRED}[FAILED]${NC}"
fi

# Offloading stuff

for SOURCE in "jacobi" "data" ; do
    if [ `arch` == "ppc64le" ]; then
	ARCHITECTURES=(nvptx64-nvidia-cuda powerpc64le-unknown-linux-gnu)
    fi
    if [ `arch` == "x86_64" ]; then
#	ARCHITECTURES=(nvptx64-nvidia-cuda x86_64-unknown-linux-gnu)
	ARCHITECTURES=(x86_64-unknown-linux-gnu)
    fi
    
    for TARGET in ${ARCHITECTURES[@]} ; do
	./${SOURCE}_$TARGET
	RET=$?
	echo -n "Running " $SOURCE " with offloading on " $TARGET
	if [ $RET == 0 ] ; then
	    echo -e "                                 ${BGREEN}[PASSED]${NC}"
	else
	    echo -e "                                 ${BRED}[FAILED]${NC}"
	fi
    done
done

echo -e "${BBLUE}Profiling interface${NC}"
cd profiling
./run.sh
cd ..

echo -e "${BBLUE}TAU selective instrumentation plugin${NC}"
cd tau
./run.sh
cd ..
