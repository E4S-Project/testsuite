#!/bin/bash -e
. ./setup.sh
set -x
for testdir in $GINKGO_DIRS #  `readlink -f */`
do
    cd ${testdir}
    ../build.sh
   cd - 
done
