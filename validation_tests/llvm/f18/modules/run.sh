#!/bin/bash

. ../setup.sh

RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'

BRED='\033[1;31m'
BGREEN='\033[1;32m'
BBLUE='\033[1;34m'

NC='\033[0m'

echo -e "${BBLUE}Module${NC}"
./use-mod1 
RC=$?
echo -n "Run the module"
if [ $RC != 0 ]; then
    echo -e "                        ${BRED}[FAILED]${NC}"
else
    echo -e "                        ${BGREEN}[PASSED]${NC}"
fi


