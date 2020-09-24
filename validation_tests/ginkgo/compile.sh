#!/bin/bash -ex
. ./setup.sh

for testdir in `readlink -f */`
do
    cd ${testdir}
    ../build.sh 
done
