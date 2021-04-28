#!/bin/bash -e
. ./setup.sh
set -x
for testdir in `readlink -f */`
do
    cd ${testdir}
    ../build.sh 
done
