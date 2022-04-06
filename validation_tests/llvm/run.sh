#!/bin/bash

if [ 1 -eq `uname -a | grep ppc64le | wc -l` ] ; then
    ARCH='power9'
else
    ARCH='x86'
fi

module use /home/users/coti/$ARCH/modulefiles

if [ $e4s_print_color = true -a  -n "$TERM" ];
then
export RED='\033[0;31m'
export GREEN='\033[0;32m'
export BLUE='\033[0;34m'

export BRED='\033[1;31m'
export BGREEN='\033[1;32m'
export BBLUE='\033[1;34m'

export NC='\033[0m'
fi
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

