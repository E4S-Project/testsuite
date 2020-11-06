#!/bin/bash -e
#-ex
. ./setup.sh
set -x
cd build

for file in `ls .` ; do                                                                                                                                   
    if [ ! -d $file ] && [ -x $file ] ; then                                                                                                                                     
        #mpirun -np 4 
	OMP_NUM_THREADS=4 ${TEST_RUN} ./$file                                                                                                                                             
    fi                                                                                                                                                         
done
cd -
