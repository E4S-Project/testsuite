#!/bin/bash

# For commented code, read cev.sh

. ./setup.sh

fIncluded=./Included
fExcluded=./Excluded
fIncludedFile=./IncludedFile
fExcludedFile=./ExcludedFile
fInstrumented=./Instrumented


RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'

BRED='\033[1;31m'
BGREEN='\033[1;32m'
BBLUE='\033[1;34m'

NC='\033[0m'

OUTFILE="toto"

runtest () {

    inputfile=$1
    executable=$2
    
    rm profile.*
    echo -e "${BBLUE}Basic instrumentation file - cpp${NC}"
    tau_exec  -T serial,clang ./$executable 256 256 &> $OUTFILE
    RC=$?
    echo -n "Execution of C instrumented code"
    if [ $RC != 0 ]; then
        echo -e "                ${BRED}[FAILED]${NC}"
    else
        echo -e "                ${BGREEN}[PASSED]${NC}"
    fi

    fExcluded=`sed '/BEGIN_EXCLUDE_LIST/,/END_EXCLUDE_LIST/{/BEGIN_EXCLUDE_LIST/{h;d};H;/END_EXCLUDE_LIST/{x;/BEGIN_EXCLUDE_LIST/,/END_EXCLUDE_LIST/p}};d' $inputfile |  sed -e 's/BEGIN_EXCLUDE_LIST//' -e 's/END_EXCLUDE_LIST//' -e '/^$/d'`
    fIncluded=`sed '/BEGIN_INCLUDE_LIST/,/END_INCLUDE_LIST/{/BEGIN_INCLUDE_LIST/{h;d};H;/END_INCLUDE_LIST/{x;/BEGIN_INCLUDE_LIST/,/END_INCLUDE_LIST/p}};d' $inputfile |  sed -e 's/BEGIN_INCLUDE_LIST//' -e 's/END_INCLUDE_LIST//'  -e '/^$/d'`
    fExcludedFile=`sed '/BEGIN_FILE_EXCLUDE_LIST/,/END_FILE_EXCLUDE_LIST/{/BEGIN_FILE_EXCLUDE_LIST/{h;d};H;/END_FILE_EXCLUDE_LIST/{x;/BEGIN_FILE_EXCLUDE_LIST/,/END_FILE_EXCLUDE_LIST/p}};d' $inputfile |  sed -e 's/BEGIN_FILE_EXCLUDE_LIST//' -e 's/END_FILE_EXCLUDE_LIST//' -e '/^$/d'`
    fIncludedFile=`sed '/BEGIN_FILE_INCLUDE_LIST/,/END_FILE_INCLUDE_LIST/{/BEGIN_FILE_INCLUDE_LIST/{h;d};H;/END_FILE_INCLUDE_LIST/{x;/BEGIN_FILE_INCLUDE_LIST/,/END_FILE_INCLUDE_LIST/p}};d' $inputfile |  sed -e 's/BEGIN_FILE_INCLUDE_LIST//' -e 's/END_FILE_INCLUDE_LIST//'  -e '/^$/d'`


    #
    #sed '/BEGIN_EXCLUDE_LIST/,/END_EXCLUDE_LIST/{/BEGIN_EXCLUDE_LIST/{h;d};H;/END_EXCLUDE_LIST/{x;/BEGIN_EXCLUDE_LIST/,/END_EXCLUDE_LIST/p}};d' ./functions_CXX_hh_files.txt |  sed -e 's/BEGIN_EXCLUDE_LIST//' -e 's/END_EXCLUDE_LIST//' -e '/^$/d' > $fExcluded
    #sed '/BEGIN_INCLUDE_LIST/,/END_INCLUDE_LIST/{/BEGIN_INCLUDE_LIST/{h;d};H;/END_INCLUDE_LIST/{x;/BEGIN_INCLUDE_LIST/,/END_INCLUDE_LIST/p}};d' ./functions_CXX_hh_files.txt |  sed -e 's/BEGIN_INCLUDE_LIST//' -e 's/END_INCLUDE_LIST//'  -e '/^$/d' > $fIncluded
    #sed '/BEGIN_FILE_EXCLUDE_LIST/,/END_FILE_EXCLUDE_LIST/{/BEGIN_FILE_EXCLUDE_LIST/{h;d};H;/END_FILE_EXCLUDE_LIST/{x;/BEGIN_FILE_EXCLUDE_LIST/,/END_FILE_EXCLUDE_LIST/p}};d' ./functions_CXX_hh_files.txt |  sed -e 's/BEGIN_FILE_EXCLUDE_LIST//' -e 's/END_FILE_EXCLUDE_LIST//' -e '/^$/d' > $fExcludedFile
    #sed '/BEGIN_FILE_INCLUDE_LIST/,/END_FILE_INCLUDE_LIST/{/BEGIN_FILE_INCLUDE_LIST/{h;d};H;/END_FILE_INCLUDE_LIST/{x;/BEGIN_FILE_INCLUDE_LIST/,/END_FILE_INCLUDE_LIST/p}};d' ./functions_CXX_hh_files.txt |  sed -e 's/BEGIN_FILE_INCLUDE_LIST//' -e 's/END_FILE_INCLUDE_LIST//'  -e '/^$/d' > $fIncludedFile


    fInstrumented=`pprof -l | grep -v "Reading" | grep -v ".TAU application"`
    
    incorrectInstrumentation=0

    for funcinstru in $fInstrumented; do
        #echo "Checking instrumentation of $line"
        varinstrumented=1
        varexcluded=1
        varfileincluded=0
        varfileexcluded=1

	# First: check that we actually wanted to instrument the instrumented functions
	# in this example we have no wildcards so the matching is straightforward
	
	#        echo "Instrumented function" $funcinstru
	if [[ $fIncluded =~ $funcinstru ]] ; then varinstrumented=0 ; fi # good
	if [[ $fExcluded =~ $funcinstru ]] ; then varexcluded=1; fi      # bad

	# Where is it defined? Look into all the source files of this directory (might pass as parameters of the function later if necessary)
	definition=`grep $funcinstru *.[ch] | grep -v ";$" | cut -d ':' -f 1`
	#	echo "Function " $funcinstru " is defined in file " $definition

	if [[ $fIncludedFile =~ $definition ]] ; then varfileincluded=0 ; fi # good
	if [[ $fExcludedFile =~ $definition ]] ; then varfileexcluded=1; fi  # bad

        if [ $varinstrumented -eq 0 ] && [ ! $varexcluded -eq 0 ] && [ $varfileincluded -eq 0 ] && [ ! $varfileexcluded -eq 0 ];
        then
            echo null > /dev/null
            echo -e "${BGREEN}Lawfully instrumented${NC}"
        elif [ $varinstrumented -eq 1 ] && [ ! $varexcluded -eq 0 ] && [ $varfileincluded -eq 0 ] && [ ! $varfileexcluded -eq 0 ];
        then
            ((incorrectInstrumentation=incorrectInstrumentation+1))
            echo -e "${BRED}Wrongfully not instrumented: included and not excluded${NC}"
        elif [ $varinstrumented -eq 0 ] && [ ! $varexcluded -eq 1 ];
        then
            ((incorrectInstrumentation=incorrectInstrumentation+1))
            echo -e "${BRED}Wrongfully instrumented: excluded${NC}"
        elif [ $varinstrumented -eq 0 ] && [ $varfileincluded -eq 1 ];
        then
            ((incorrectInstrumentation=incorrectInstrumentation+1))
            echo -e "${BRED}Wrongfully instrumented: source file is not included${NC}"
        elif [ $varinstrumented -eq 0 ] && [ ! $varfileexcluded -eq 1 ];
        then
            ((incorrectInstrumentation=incorrectInstrumentation+1))
            echo -e "${BRED}Wrongfully instrumented: source file is excluded${NC}"
        elif [ $varinstrumented -eq 1 ] && ([ ! $varexcluded -eq 1 ] || [ $varfileincluded -eq 1 ] || [ ! $varfileexcluded -eq 1 ]);
        then
            echo null > /dev/null
            echo -e "${BGREEN}Lawfully not instrumented: excluded or not included${NC}"
        else
            echo Uncovered case to implement 
            ((incorrectInstrumentation=incorrectInstrumentation+1))
        fi

    done

    for funcinstru in $fInstrumented; do
        #echo "Checking inclusion of $line"
        varincluded=0
        if echo $line | grep -qF "TAU";
        then
            continue
        fi
	if [[ $fIncluded =~ $funcinstru ]] ; then varincluded=1; fi  

        if [ $varincluded -gt 0 ];
        then
            ((incorrectInstrumentation=incorrectInstrumentation+1))
            echo -e "${BRED}Wrongfully instrumented, not included${NC}"
        fi
    done 

    return $incorrectInstrumentation
}

runtest "functions_C_files.txt" "householder3"
runoutput=$?

echo -n "Instrumentation of C code"
if [ $runoutput -eq 0 ]; then
    echo -e "                       ${BGREEN}[PASSED]${NC}"
else
    echo -e "                       ${BRED}[FAILED]${NC}"
fi



rm $OUTFILE
#rm profile.*
