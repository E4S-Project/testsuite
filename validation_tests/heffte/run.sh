#!/bin/bash 
#-ex
. ./setup.sh
cd build

for file in `ls .` ; do                                                                                                                                   
    if [ ! -d $file ] && [ -x $file ] ; then                                                                                                                                     
        #mpirun -np 4 
	eval $TEST_RUN $file                                                                                                                                             
    fi                                                                                                                                                         
done
cd -
