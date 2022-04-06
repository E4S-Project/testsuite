#!/bin/bash

. ../setup.sh

#RED='\033[0;31m'
#GREEN='\033[0;32m'
#BLUE='\033[0;34m'

#BRED='\033[1;31m'
#BGREEN='\033[1;32m'
#BBLUE='\033[1;34m'

#NC='\033[0m'

echo -e "${BBLUE}Module${NC}"
f18 -module-suffix .f18.mod -c mod1.f90
RC=$?
echo -n "Compile the module"
if [ $RC != 0 ]; then
    echo -e "                    ${BRED}[FAILED]${NC}"
else
    echo -e "                    ${BGREEN}[PASSED]${NC}"
fi

f18 -module-suffix .f18.mod -o use-mod1 use-mod1.f90
RC=$?
echo -n "Use the module"
if [ $RC != 0 ]; then
    echo -e "                        ${BRED}[FAILED]${NC}"
else
    echo -e "                        ${BGREEN}[PASSED]${NC}"
fi


