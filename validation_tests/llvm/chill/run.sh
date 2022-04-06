#!/bin/bash

. ./setup.sh

#RED='\033[0;31m'
#GREEN='\033[0;32m'
#BLUE='\033[0;34m'

#BRED='\033[1;31m'
#BGREEN='\033[1;32m'
#BBLUE='\033[1;34m'

#NC='\033[0m'

function start_chill(){
    
    # Verification: can we start CHiLL
    
    for EXEC in "chill" "cuda-chill" ; do
	echo "exit()" | $EXEC > /dev/null
	RET=$?
	echo -n "Start :" $EXEC "     "
	if [ $RET == 0 ] ; then
	    echo -e "                  ${BGREEN}[PASSED]${NC}"
	else
	    echo -e "                  ${BRED}[FAILED]${NC}"
	fi
    done    
}

function load_nonexistent(){

    # When we try to load a file that does not exist, CHiLL segfaults
    
    chill nofile.py &> /dev/null
    RET=$?
    echo -n "Segmentation fault when trying to load a non-existent file:   "
    if [ $RET == 139 ] ; then
	echo -e "                  ${BGREEN}[PASSED]${NC}"
    else
	echo -e "                  ${BRED}[FAILED]${NC}"
    fi
}

function run_example(){

    # Run a simple example

    CHILLSCRIPT=$1
    INPUTFILE=$2
    TESTMAIN=$3
    
    chill $CHILLSCRIPT &> /dev/null
    RET=$?
    echo -n "test :           "
    if [ $RET == 0 ] ; then
	echo -e "                  ${BGREEN}[PASSED]${NC}"
    else
	echo -e "                  ${BRED}[FAILED]${NC}"
    fi
    # It must have produced an output file
    echo -n "Output file present:    "
    if test -f "src/${INPUTFILE}_modified.c"; then   
	echo -e "                  ${BGREEN}[PASSED]${NC}"
    else
	echo -e "                  ${BRED}[FAILED]${NC}"
    fi
    # Does it give the same result
    cd testfiles
    echo -n "Produced code gives the same result:   "
    gcc -o $TESTMAIN ${TESTMAIN}.c ../src/${INPUTFILE}_modified.c
    ./$TESTMAIN > tmp2
    gcc -o $TESTMAIN ${TESTMAIN}.c ../src/${INPUTFILE}.c
    ./$TESTMAIN > tmp
    TOTO=`diff tmp tmp2`
    if [ -z $TOTO ]; then
	echo -e "                  ${BGREEN}[PASSED]${NC}"
    else
	echo -e "                  ${BRED}[FAILED]${NC}"
    fi
    rm tmp tmp2
    cd ..
}

echo -e "${BBLUE}Sanity check${NC}"
start_chill
load_nonexistent
echo -n "Simple "
run_example simple.py singleloop single_main
echo -e "${BBLUE}Loop modifications${NC}"
echo -n "Nested loops "
run_example nested.py nestedloops nested_main
echo -n "Reorder loops "
run_example nested_swap.py nestedloops nested_main
echo -n "Tile loops "
run_example nested_tiling.py nestedloops nested_main
echo -n "Reverse loops "
run_example reverse1.script.py mm mm_main
echo -n "Peel loops "
run_example nested_peel.py mm mm_main


# gcc -g -Wall -o hh_main hh_main.c ../src/householder2.c -lm

cd hh
./run.sh
cd ..
