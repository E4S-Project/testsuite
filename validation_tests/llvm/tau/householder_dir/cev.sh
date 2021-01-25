#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'

BRED='\033[1;31m'
BGREEN='\033[1;32m'
BBLUE='\033[1;34m'

NC='\033[0m'

. ./setup.sh
# TAU_MAKEFILE represent the specific installation of TAU
export TAU_MAKEFILE=shared-TEST-clang
export LLVM_DIR=/home/users/fdeny/llvm_build/pluginVersions/plugin-tau-llvm-inuse/install
ERRFILE="toto"

EXECUTABLE=householder

if [ $# -ne 1 ]; then
    echo "Missing input file: stopping the test"
    exit
fi

if [ $(cat $1 | wc -l) -eq 0 ]; then
    echo -e "Input file doesn't exist: stopping the test"
    exit
else
    echo -e "Input file detected"
fi

echo -e "${BBLUE}Instrumentation${NC}"


# Compilation is done in two parts, in order to keep the .o executables files as to know
# where each function is implemented
clang++ -c -O3 -g -fplugin=${LLVM_DIR}/lib/TAU_Profiling_CXX.so -mllvm -tau-input-file=./$1 householder.cpp R.cpp Q.cpp matmul.cpp &> $ERRFILE

clang++ -o $EXECUTABLE householder.o R.o Q.o matmul.o -fplugin=${LLVM_DIR}/lib/TAU_Profiling_CXX.so -ldl -L${TAU}/lib/$TAU_MAKEFILE -lTAU -Wl,-rpath,${TAU}/lib/$TAU_MAKEFILE

# We check that the return code of the compilation is indeed 0
RC=$?
echo -n "C++ instrumentation"
if [ $RC != 0 ]; then
    echo -e "                               ${BRED}[FAILED]${NC}"
    echo -e "Compilation failed: stopping the test"
    exit
else
    echo -e "                               ${BGREEN}[PASSED]${NC}"
fi

# We check that the output of the compilation mentions the instumentation of functions
echo -n "Instrumented functions"
if [ `grep "Instrument"  $ERRFILE | wc -l` -gt 0 ] ; then
    echo -e "                            ${BGREEN}[PASSED]${NC}"
else
    echo -e "                            ${BRED}[FAILED]${NC}"
fi
rm $ERRFILE


# The files here will contain the list of function/files that are *supposed* to be
# included/excluded
fIncluded=./Included
fExcluded=./Excluded
fIncludedFile=./IncludedFile
fExcludedFile=./ExcludedFile

# This file contain the list of functions that were *indeed* instrumented
fInstrumented=./Instrumented

BRED='\033[1;31m'
BGREEN='\033[1;32m'

NC='\033[0m'


# We parse the input file to know which function/file is supposed to be included/excluded
sed '/BEGIN_EXCLUDE_LIST/,/END_EXCLUDE_LIST/{/BEGIN_EXCLUDE_LIST/{h;d};H;/END_EXCLUDE_LIST/{x;/BEGIN_EXCLUDE_LIST/,/END_EXCLUDE_LIST/p}};d' $1 |  sed -e 's/BEGIN_EXCLUDE_LIST//' -e 's/END_EXCLUDE_LIST//' -e '/^$/d' > $fExcluded
sed '/BEGIN_INCLUDE_LIST/,/END_INCLUDE_LIST/{/BEGIN_INCLUDE_LIST/{h;d};H;/END_INCLUDE_LIST/{x;/BEGIN_INCLUDE_LIST/,/END_INCLUDE_LIST/p}};d' $1 |  sed -e 's/BEGIN_INCLUDE_LIST//' -e 's/END_INCLUDE_LIST//'  -e '/^$/d' > $fIncluded 
sed '/BEGIN_FILE_EXCLUDE_LIST/,/END_FILE_EXCLUDE_LIST/{/BEGIN_FILE_EXCLUDE_LIST/{h;d};H;/END_FILE_EXCLUDE_LIST/{x;/BEGIN_FILE_EXCLUDE_LIST/,/END_FILE_EXCLUDE_LIST/p}};d' $1 |  sed -e 's/BEGIN_FILE_EXCLUDE_LIST//' -e 's/END_FILE_EXCLUDE_LIST//' -e '/^$/d' > $fExcludedFile 
sed '/BEGIN_FILE_INCLUDE_LIST/,/END_FILE_INCLUDE_LIST/{/BEGIN_FILE_INCLUDE_LIST/{h;d};H;/END_FILE_INCLUDE_LIST/{x;/BEGIN_FILE_INCLUDE_LIST/,/END_FILE_INCLUDE_LIST/p}};d' $1 |  sed -e 's/BEGIN_FILE_INCLUDE_LIST//' -e 's/END_FILE_INCLUDE_LIST//'  -e '/^$/d' > $fIncludedFile


# We execute the compiled executable with TAU
tau_exec  -T serial,clang ./$EXECUTABLE
# And gather which functions were indeed instrumented
pprof -l | grep -v "Reading" > $fInstrumented
# We have a error counter to check for the presence of any mistakes at the end of the script
incorrectInstrumentation=0



# This while loop goes through the functions that were written as included
# (WARNING: being written as included doesn't mean the function should ultimately be
# instrumented, depending on the other inclusions/exclusions)
while read -r line ; do
    echo "Checking instrumentation of $line"
    varinstrumented=1 # 1 means the current function wasn't instrumented
    varexcluded=1 # 1 means the current function wasn't excluded
    varfileincluded=0  # 0 means the current function's implementing file is included 
    varfileexcluded=1  # 1 means the current function's implementing file isn't excluded 
    
    # We inquire as to if the current function was indeed instrumented
    grep -qF  "$line" $fInstrumented;
    varinstrumented=$?

    # We inquire as to if the current function was excluded
    grep -qF "$line" $fExcluded;
    varexcluded=$?

    # We inquire as to if the current function was implemented in an excluded file 
    # We loop through the exluded files to do so
    while read -r linefile ; do
        newlinefile="${linefile%.*}.o" # Change file's name to .o
        # We inquire as to if the function is mentioned in the defined symbols of 
        # the excluded .o executable, to know if the original file
        # implements the function 
        if nm -C --defined-only $newlinefile | grep -qFw "$line";
        then
            echo "File is excluded"
            varfileexcluded=0
        fi
    done < $fExcludedFile
    
    # We inquire as to if the current function was implemented in an included file 
    # (if no file was included then all are included) 
    if [ $(cat $fIncludedFile | wc -l) -gt 0 ];
    then
        varfileincluded=1
        # This loops works similarly as the varfileexcluded one
        while read -r linefile ; do
            newlinefile="${linefile%.*}.o"
            if nm -C --defined-only $newlinefile | grep -qFw "$line";
            then
                varfileincluded=0
            fi
        done < $fIncludedFile
    fi

    # We compare the gathered values and draw a conclusion on the validity of 
    # the instrumentation/non-instrumentation of the function 
    if [ $varinstrumented -eq 0 ] && [ ! $varexcluded -eq 0 ] && [ $varfileincluded -eq 0 ] && [ ! $varfileexcluded -eq 0 ];
    then
        echo -e "${BGREEN}Lawfully instrumented${NC}"
    if [ $varinstrumented -eq 1 ] && [ ! $varexcluded -eq 0 ] && [ $varfileincluded -eq 0 ] && [ ! $varfileexcluded -eq 0 ];
    then
        ((incorrectInstrumentation=incorrectInstrumentation+1))
        echo -e "${BRED}Wrongfully not instrumented: included and not excluded${NC}"
    fi
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
        echo -e "${BGREEN}Lawfully not instrumented: excluded or not included${NC}"
    else
        echo Uncovered case to implement 
    fi

done < $fIncluded

# This while loop goes through every function that were instrumented,to check if they
# were duly included (This covers the remaining cases the first loop misses).
while read -r line ; do
    varincluded=0
    if echo $line | grep -q "TAU"; # We go though this line that doesn't refer to a function
    then
        continue
    fi
    echo "Checking inclusion of $line"
    grep -qF "$line" $fIncluded; # We check if the function was indeed included
    varincluded=$?

    if [ $varincluded -gt 0 ];
    then
        ((incorrectInstrumentation=incorrectInstrumentation+1))
        echo -e "${BRED}Wrongfully instrumented, not included${NC}"
    fi
done < $fInstrumented


# We check the incorrect instrumantation counter and conclude
if [ $incorrectInstrumentation -eq 0 ]; then
    echo -e "${BGREEN}[Instrumentation done correctly]${NC}"
else
    echo -e "${BRED}[Instrumentation done incorrectly: $incorrectInstrumentation mistakes]${NC}"
fi
#rm profile.*
rm $fIncluded
rm $fIncludedFile
rm $fExcluded
rm $fExcludedFile
rm $fInstrumented
