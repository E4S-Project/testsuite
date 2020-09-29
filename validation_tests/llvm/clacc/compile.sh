#!/bin/bash

. ./setup.sh

RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'

BRED='\033[1;31m'
BGREEN='\033[1;32m'
BBLUE='\033[1;34m'

NC='\033[0m'

# Compilation flags tests
echo -e "${BBLUE}Compilation flags${NC}"

clang -fopenacc -o parallel parallel.c
RET=$?
echo -n "Simple compilation  " 
if [ $RET == 0 ] ; then
    echo -e "                                 ${BGREEN}[PASSED]${NC}"
else
    echo -e "                                 ${BRED}[FAILED]${NC}"
fi
clang -fno-openacc -o parallel parallel.c
OUTFILE=toto
ERRFILE=titi
for FLAG in "-fopenacc-print=acc" "-fopenacc-print=omp " "-fopenacc-print=acc-omp " "-fopenacc-print=omp-acc" ; do
    clang $FLAG -o parallel parallel.c > $OUTFILE
    RET=$?
    echo -n $FLAG "  "
    if [ $RET == 0 ] ; then
	echo -e "                               ${BGREEN}[PASSED]${NC}"
    else
	echo -e "                               ${BRED}[FAILED]${NC}"
    fi
done

clang -Wsource-uses-openacc -o parallel parallel.c > $OUTFILE 2> $ERRFILE
RET=$?
echo -n "-Wsource-uses-openacc  " 
if [ $RET == 0 ] ; then
    if [ `grep warning $ERRFILE | wc -l` -gt 0 ] ; then
	echo -e "                              ${BGREEN}[PASSED]${NC}"
    else
	echo -e "                              ${BRED}[FAILED]${NC} compiled but no warning generated"
    fi
else
    echo -e "                                 ${BRED}[FAILED]${NC}"
fi
clang -fopenacc -Wopenacc-ignored-clause -o ignore ignore.c > $OUTFILE 2> $ERRFILE
RET=$?
echo -n "-Wopenacc-ignored-clause  " 
if [ $RET == 0 ] ; then
    if [ `grep warning $ERRFILE | wc -l` -gt 0 ] ; then
	echo -e "                                 ${BGREEN}[PASSED]${NC}"
    else
	echo -e "                                 ${BRED}[FAILED]${NC} compiled but no warning generated"
    fi
else
    echo -e "                                 ${BRED}[FAILED]${NC}"
fi


#-fopenmp-targets=<triples> for traditional compilation mode
# nvptx64-nvidia-cuda
# powerpc64le-unknown-linux-gnu
# x86_64-unknown-linux-gnu
echo -e "${BBLUE}Offloading${NC}"

for SOURCE in "jacobi" "data" ; do
    if [ `arch` == "ppc64le" ]; then
	ARCHITECTURES=(nvptx64-nvidia-cuda powerpc64le-unknown-linux-gnu)
    fi
    if [ `arch` == "x86_64" ]; then
#	ARCHITECTURES=(nvptx64-nvidia-cuda x86_64-unknown-linux-gnu)
	ARCHITECTURES=(x86_64-unknown-linux-gnu)
    fi
    
    for TARGET in ${ARCHITECTURES[@]} ; do
	clang -O3 -fopenacc -fopenmp-targets=$TARGET -o ${SOURCE}_$TARGET ${SOURCE}.c
	RET=$?
	echo -n "Compile with target " $TARGET
	if [ $RET == 0 ] ; then
	    echo -e "                  ${BGREEN}[PASSED]${NC}"
	else
	    echo -e "                  ${BRED}[FAILED]${NC}"
	fi
    done
done

# Compile things we are going to run

echo -e "${BBLUE}Misc features${NC}"
clang -fopenacc -o parallel parallel.c
RC=$?
echo -n "Parallel"
if [ $RC != 0 ]; then
    echo -e "                               ${BRED}[FAILED]${NC}"
else
    echo -e "                               ${BGREEN}[PASSED]${NC}"
fi
clang -fopenacc -o gang gang.c
RC=$?
echo -n "Gang"
if [ $RC != 0 ]; then
    echo -e "                                   ${BRED}[FAILED]${NC}"
else
    echo -e "                                   ${BGREEN}[PASSED]${NC}"
fi

# Small tests

clang -fopenacc -Wall -o inout inout.c 
RC=$?
echo -n "In/out"
if [ $RC != 0 ]; then
    echo -e "                                 ${BRED}[FAILED]${NC}"
else
    echo -e "                                 ${BGREEN}[PASSED]${NC}"
fi
clang -fopenacc -Wall -o inout_data inout_data.c 
RC=$?
echo -n "In/out data"
if [ $RC != 0 ]; then
    echo -e "                            ${BRED}[FAILED]${NC}"
else
    echo -e "                            ${BGREEN}[PASSED]${NC}"
fi

clang -fopenacc -Wall -o jacobi jacobi.c 
RC=$?
echo -n "Jacobi"
if [ $RC != 0 ]; then
    echo -e "                                 ${BRED}[FAILED]${NC}"
else
    echo -e "                                 ${BGREEN}[PASSED]${NC}"
fi
clang -fopenacc -Wall -o jacobi_data jacobi_data.c 
RC=$?
echo -n "Jacobi_data"
if [ $RC != 0 ]; then
    echo -e "                            ${BRED}[FAILED]${NC}"
else
    echo -e "                            ${BGREEN}[PASSED]${NC}"
fi

#make
clang -O3 -fopenacc -o householder3 householder3.c -lm
RC=$?
echo -n "Householder"
if [ $RC != 0 ]; then
    echo -e "                            ${BRED}[FAILED]${NC}"
else
    echo -e "                            ${BGREEN}[PASSED]${NC}"
fi

echo -e "${BBLUE}Profiling interface${NC}"
cd profiling
./compile.sh
cd ..

echo -e "${BBLUE}TAU selective instrumentation plugin${NC}"
cd tau
./compile.sh
cd ..
