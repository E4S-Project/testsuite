#!/bin/bash


BRED='\033[1;31m'
BGREEN='\033[1;32m'

NC='\033[0m'
sed '/BEGIN_EXCLUDE_LIST/,/END_EXCLUDE_LIST/{/BEGIN_EXCLUDE_LIST/{h;d};H;/END_EXCLUDE_LIST/{x;/BEGIN_EXCLUDE_LIST/,/END_EXCLUDE_LIST/p}};d' $1 |  sed -e 's/BEGIN_EXCLUDE_LIST//' -e 's/END_EXCLUDE_LIST//' -e '/^$/d' > Excluded
sed '/BEGIN_INCLUDE_LIST/,/END_INCLUDE_LIST/{/BEGIN_INCLUDE_LIST/{h;d};H;/END_INCLUDE_LIST/{x;/BEGIN_INCLUDE_LIST/,/END_INCLUDE_LIST/p}};d' $1 |  sed -e 's/BEGIN_INCLUDE_LIST//' -e 's/END_INCLUDE_LIST//'  -e '/^$/d' > Included

fIncluded=./Included
fExcluded=./Excluded

tau_exec  -T serial,clang $2 

pprof -l | grep -v "Reading" > Instrumented

fInstrumented=./Instrumented
incorrectInstrumentation=0



while read -r line ; do
    echo "Checking intrumentation of $line"
    if grep -qF  "$line" $fInstrumented;
    then
        echo -e "${BGREEN}Instrumented${NC}"
    else
        if grep -qF "$line" $fExcluded; then
	    echo -e "${BRED}Excluded${NC}"
        else
	    if echo $line | grep -q "#" ;then
	        echo "'#' symbol"
	    else    
                echo -e "${BRED}Function wrongfully not instrumented${NC}"
		((incorrectInstrumentation=incorrectInstrumentation+1))
	    fi
	fi
    fi
done < $fIncluded

while read -r line ; do
    echo "Checking exclusion of $line"
    if grep -qF  "$line" $fInstrumented;
    then
        echo -e "${BRED}Function wrongfully instrumented${NC}"
	((incorrectInstrumentation=incorrectInstrumentation+1))
    else
	echo -e "${BGREEN}Excluded${NC}"
    fi
done < $fExcluded

if [ $incorrectInstrumentation -eq 0 ]; then
    echo -e "${BGREEN}[Instrumentation done correctly]${NC}"
else
    echo -e "${BRED}[Instrumentation done incorrectly: $incorrectInstrumentation mistakes]${NC}"
fi
