#!/bin/bash

LD_PRELOAD=./profiling.so ./basic2 &> /dev/null
RC=$?
echo -n "Profiling basic2"
if [ $RC != 0 ]; then
    echo -e "                            ${BRED}[FAILED]${NC}"
else
    echo -e "                            ${BGREEN}[PASSED]${NC}"
fi

if [ `arch` == "ppc64le" ]; then
    ARCHITECTURES=(nvptx64-nvidia-cuda powerpc64le-unknown-linux-gnu)
fi
if [ `arch` == "x86_64" ]; then
#    ARCHITECTURES=(nvptx64-nvidia-cuda x86_64-unknown-linux-gnu)
    ARCHITECTURES=(x86_64-unknown-linux-gnu)
fi
for TARGET in ${ARCHITECTURES[@]} ; do
    LD_PRELOAD=./profiling.so ./jacobi_off_$TARGET &> /dev/null
    RC=$?
    echo -n "Profiling ./jacobi_off_$TARGET"
    if [ $RC != 0 ]; then
	echo -e "                            ${BRED}[FAILED]${NC}"
    else
	echo -e "                            ${BGREEN}[PASSED]${NC}"
    fi
done

LD_PRELOAD=./profiling.so ../householder3 64 64 &> /dev/null
RC=$?
echo -n "Profiling Householder3"
if [ $RC != 0 ]; then
    echo -e "                            ${BRED}[FAILED]${NC}"
else
    echo -e "                            ${BGREEN}[PASSED]${NC}"
fi



