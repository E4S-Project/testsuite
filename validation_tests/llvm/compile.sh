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

for d in `cat testdirs.txt` ; do
    echo -e "${BLUE}   Compiling ${d} ${NC}    "
    cd $d
    ./clean.sh
    ./compile.sh
    cd $CURRENTDIR
done

