#!/bin/bash


fIncluded=./Included
fExcluded=./Excluded
fIncludedFile=./IncludedFile
fExcludedFile=./ExcludedFile
fInstrumented=./Instrumented

BRED='\033[1;31m'
BGREEN='\033[1;32m'

NC='\033[0m'

sed '/BEGIN_EXCLUDE_LIST/,/END_EXCLUDE_LIST/{/BEGIN_EXCLUDE_LIST/{h;d};H;/END_EXCLUDE_LIST/{x;/BEGIN_EXCLUDE_LIST/,/END_EXCLUDE_LIST/p}};d' $1 |  sed -e 's/BEGIN_EXCLUDE_LIST//' -e 's/END_EXCLUDE_LIST//' -e '/^$/d' > $fExcluded
sed '/BEGIN_INCLUDE_LIST/,/END_INCLUDE_LIST/{/BEGIN_INCLUDE_LIST/{h;d};H;/END_INCLUDE_LIST/{x;/BEGIN_INCLUDE_LIST/,/END_INCLUDE_LIST/p}};d' $1 |  sed -e 's/BEGIN_INCLUDE_LIST//' -e 's/END_INCLUDE_LIST//'  -e '/^$/d' > $fIncluded
sed '/BEGIN_FILE_EXCLUDE_LIST/,/END_FILE_EXCLUDE_LIST/{/BEGIN_FILE_EXCLUDE_LIST/{h;d};H;/END_FILE_EXCLUDE_LIST/{x;/BEGIN_FILE_EXCLUDE_LIST/,/END_FILE_EXCLUDE_LIST/p}};d' $1 |  sed -e 's/BEGIN_FILE_EXCLUDE_LIST//' -e 's/END_FILE_EXCLUDE_LIST//' -e '/^$/d' > $fExcludedFile
sed '/BEGIN_FILE_INCLUDE_LIST/,/END_FILE_INCLUDE_LIST/{/BEGIN_FILE_INCLUDE_LIST/{h;d};H;/END_FILE_INCLUDE_LIST/{x;/BEGIN_FILE_INCLUDE_LIST/,/END_FILE_INCLUDE_LIST/p}};d' $1 |  sed -e 's/BEGIN_FILE_INCLUDE_LIST//' -e 's/END_FILE_INCLUDE_LIST//'  -e '/^$/d' > $fIncludedFile



tau_exec  -T serial,clang $2

pprof -l | grep -v "Reading" > $fInstrumented

incorrectInstrumentation=0




while read -r line ; do
    echo "Checking intrumentation of $line"
    varinstrumented=1
    varexcluded=1
    varfileincluded=0
    varfileexcluded=1

    grep -qw  "$line" $fInstrumented;
    varinstrumented=$?

    grep -qw "$line" $fExcluded;
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
    echo $varinstrumented
    echo $varexcluded
    echo $varfileincluded
    echo $varfileexcluded


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
    elif [ $varinstrumented -eq 1 ] && ([ ! $varexcluded -eq 1 ] || [ $varfileincluded -eq 1 ] || [ ! $varfileexcluded -eq 1]);
    then
        echo -e "${BGREEN}Lawfully not instrumented: excluded or not included${NC}"
    else
        echo Uncovered case to implement 
    fi

done < $fIncluded


while read -r line ; do
    echo "Checking intrumentation of $line"
    varincluded=0
    if echo $line | grep -q "TAU";
    then
        continue
    fi
    grep -qw "$line" $fIncluded;
    varincluded=$?

    if [ $varincluded -gt 0 ];
    then
        ((incorrectInstrumentation=incorrectInstrumentation+1))
        echo -e "${BRED}Wrongfully instrumented, not included${NC}"
    fi
done < $fInstrumented

if [ $incorrectInstrumentation -eq 0 ]; then
    echo -e "${BGREEN}[Instrumentation done correctly]${NC}"
else
    echo -e "${BRED}[Instrumentation done incorrectly: $incorrectInstrumentation mistakes]${NC}"
fi

