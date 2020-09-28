#!/bin/bash

. ../setup.sh

crout/crout > /dev/null
RC=$?
echo -n "Crout   "
if [ $? != 0 ]; then
    echo -e "                                 ${BRED}[FAILED]${NC}"
else
    echo -e "                                 ${BGREEN}[PASSED]${NC}"
fi

