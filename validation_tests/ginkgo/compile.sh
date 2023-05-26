#!/bin/bash -e
. ./setup.sh
set -x

export CXX=$TEST_CXX_MPI
for testdir in $GINKGO_DIRS #  `readlink -f */`
do
    cd ${testdir}
    ../build.sh
   cd - 
done
