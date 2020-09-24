#!/bin/bash

for testdir in `readlink -f */`
do
    cd ${testdir}
    rm -f `basename ${testdir}`
    rm -rf doc CMakeLists.txt my_file.txt log.txt
done


