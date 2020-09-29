#!/bin/bash

rm profile.*

tau_exec -T serial,clang,acc ./householder3 &> /dev/null
RC=$?
echo -n "Execution"
if [ $RC != 0 ]; then
    echo -e "                                   ${BRED}[FAILED]${NC}"
else
    echo -e "                                   ${BGREEN}[PASSED]${NC}"
fi

echo -n "Creation of a profile file:"
if test -f "profile.0.0.0"; then
    echo -e "                 ${BGREEN}[PASSED]${NC}"
else
    echo -e "                 ${BRED}[FAILED]${NC}"
fi

echo -n "Exploitation of the profile file:"
pprof &> /dev/null
if [ $? != 0 ]; then
    echo -e "           ${BRED}[FAILED]${NC}"
else
    echo -e "           ${BGREEN}[PASSED]${NC}"
fi
