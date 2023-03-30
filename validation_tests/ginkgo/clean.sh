#!/bin/bash
. ./setup.sh
for testdir in $GINKGO_DIRS  #`readlink -f */`
do
    cd ${testdir}
    rm -f `basename ${testdir}`
    rm -rf doc CMakeLists.txt my_file.txt log.txt
    cd .. 
done


