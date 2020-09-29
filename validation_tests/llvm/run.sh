#!/bin/bash

if [ 1 -eq `uname -a | grep ppc64le | wc -l` ] ; then
    ARCH='power9'
else
    ARCH='x86'
fi

module use /home/users/coti/$ARCH/modulefiles


RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'

BRED='\033[1;31m'
BGREEN='\033[1;32m'
BBLUE='\033[1;34m'

NC='\033[0m'

CURRENTDIR=`pwd`
OUTPUTFILE=$CURRENTDIR/"run.log"

for d in `cat testdirs.txt` ; do
    TMPFILE=/tmp/toto
    echo -e "${BLUE}   Running ${d} ${NC}    " | tee -a $OUTPUTFILE
    cd $d
    ./run.sh &> $TMPFILE
    passed=`grep "PASSED" $TMPFILE | wc -l`
    failed=`grep "FAILED" $TMPFILE | wc -l`
    echo "Passed:   " $passed"/"$(($passed+$failed))
    cat $TMPFILE >> $OUTPUTFILE
    rm $TMPFILE
    cd $CURRENTDIR
done

