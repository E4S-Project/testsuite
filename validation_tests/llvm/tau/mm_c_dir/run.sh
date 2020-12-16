#!/bin/bash

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


rm profile.*
echo -e "${BBLUE}Basic instrumentation file - c${NC}"
tau_exec  -T serial,clang ./mm_c 256 256 &> $OUTFILE
RC=$?
echo -n "Execution of C instrumented code"
if [ $RC != 0 ]; then
    echo -e "                  ${BRED}[FAILED]${NC}"
else
    echo -e "                  ${BGREEN}[PASSED]${NC}"
fi


sed '/BEGIN_EXCLUDE_LIST/,/END_EXCLUDE_LIST/{/BEGIN_EXCLUDE_LIST/{h;d};H;/END_EXCLUDE_LIST/{x;/BEGIN_EXCLUDE_LIST/,/END_EXCLUDE_LIST/p}};d' ./functions_C_mm.txt |  sed -e 's/BEGIN_EXCLUDE_LIST//' -e 's/END_EXCLUDE_LIST//' -e '/^$/d' > $fExcluded
sed '/BEGIN_INCLUDE_LIST/,/END_INCLUDE_LIST/{/BEGIN_INCLUDE_LIST/{h;d};H;/END_INCLUDE_LIST/{x;/BEGIN_INCLUDE_LIST/,/END_INCLUDE_LIST/p}};d' ./functions_C_mm.txt |  sed -e 's/BEGIN_INCLUDE_LIST//' -e 's/END_INCLUDE_LIST//'  -e '/^$/d' > $fIncluded
sed '/BEGIN_FILE_EXCLUDE_LIST/,/END_FILE_EXCLUDE_LIST/{/BEGIN_FILE_EXCLUDE_LIST/{h;d};H;/END_FILE_EXCLUDE_LIST/{x;/BEGIN_FILE_EXCLUDE_LIST/,/END_FILE_EXCLUDE_LIST/p}};d' ./functions_C_mm.txt |  sed -e 's/BEGIN_FILE_EXCLUDE_LIST//' -e 's/END_FILE_EXCLUDE_LIST//' -e '/^$/d' > $fExcludedFile
sed '/BEGIN_FILE_INCLUDE_LIST/,/END_FILE_INCLUDE_LIST/{/BEGIN_FILE_INCLUDE_LIST/{h;d};H;/END_FILE_INCLUDE_LIST/{x;/BEGIN_FILE_INCLUDE_LIST/,/END_FILE_INCLUDE_LIST/p}};d' ./functions_C_mm.txt |  sed -e 's/BEGIN_FILE_INCLUDE_LIST//' -e 's/END_FILE_INCLUDE_LIST//'  -e '/^$/d' > $fIncludedFile


pprof -l | grep -v "Reading" > $fInstrumented

incorrectInstrumentation=0
while read -r line ; do
    #echo "Checking instrumentation of $line"
    varinstrumented=1
    varexcluded=1
    varfileincluded=0
    varfileexcluded=1


    grep -qF  "$line" $fInstrumented;
    varinstrumented=$?

    grep -qF "$line" $fExcluded;
    varexcluded=$?

    while read -r linefile ; do
        newlinefile="${linefile%.*}.o"
        if nm --defined-only $newlinefile | grep -qw "$line";
        then
            varfileexcluded=0
        fi
    done < $fExcludedFile

    if [ $(cat $fIncludedFile | wc -l) -gt 0 ];
    then
        varfileincluded=1
        while read -r linefile ; do
            newlinefile="${linefile%.*}.o"
            if nm --defined-only $newlinefile | grep -qw "$line";
            then
                varfileincluded=0
            fi
        done < $fIncludedFile
    fi
    #echo $varinstrumented
    #echo $varexcluded
    #echo $varfileincluded
    #echo $varfileexcluded


    if [ $varinstrumented -eq 0 ] && [ ! $varexcluded -eq 0 ] && [ $varfileincluded -eq 0 ] && [ ! $varfileexcluded -eq 0 ];
    then
        echo null > /dev/null
        #echo -e "${BGREEN}Lawfully instrumented${NC}"
    elif [ $varinstrumented -eq 1 ] && [ ! $varexcluded -eq 0 ] && [ $varfileincluded -eq 0 ] && [ ! $varfileexcluded -eq 0 ];
    then
        ((incorrectInstrumentation=incorrectInstrumentation+1))
        #echo -e "${BRED}Wrongfully not instrumented: included and not excluded${NC}"
    elif [ $varinstrumented -eq 0 ] && [ ! $varexcluded -eq 1 ];
    then
        ((incorrectInstrumentation=incorrectInstrumentation+1))
        #echo -e "${BRED}Wrongfully instrumented: excluded${NC}"
    elif [ $varinstrumented -eq 0 ] && [ $varfileincluded -eq 1 ];
    then
        ((incorrectInstrumentation=incorrectInstrumentation+1))
        #echo -e "${BRED}Wrongfully instrumented: source file is not included${NC}"
    elif [ $varinstrumented -eq 0 ] && [ ! $varfileexcluded -eq 1 ];
    then
        ((incorrectInstrumentation=incorrectInstrumentation+1))
        #echo -e "${BRED}Wrongfully instrumented: source file is excluded${NC}"
    elif [ $varinstrumented -eq 1 ] && ([ ! $varexcluded -eq 1 ] || [ $varfileincluded -eq 1 ] || [ ! $varfileexcluded -eq 1]);
    then
        echo null > /dev/null
        #echo -e "${BGREEN}Lawfully not instrumented: excluded or not included${NC}"
    else
        echo Uncovered case to implement 
        ((incorrectInstrumentation=incorrectInstrumentation+1))
    fi

done < $fIncluded

while read -r line ; do
    #echo "Checking inclusion of $line"
    varincluded=0
    if echo $line | grep -qF "TAU";
    then
        continue
    fi
    grep -qwF "$line" $fIncluded;
    varincluded=$?

    if [ $varincluded -gt 0 ];
    then
        ((incorrectInstrumentation=incorrectInstrumentation+1))
       #echo -e "${BRED}Wrongfully instrumented, not included${NC}"
    fi
done < $fInstrumented

echo -n "Instrumentation of C code"
if [ $incorrectInstrumentation -eq 0 ]; then
    echo -e "                         ${BGREEN}[PASSED]${NC}"
else
    echo -e "                         ${BRED}[FAILED]${NC}"
fi



rm $OUTFILE
rm profile.*
rm $fIncluded
rm $fIncludedFile
rm $fExcluded
rm $fExcludedFile
rm $fInstrumented

