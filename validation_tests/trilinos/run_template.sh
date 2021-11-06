#!/bin/bash
. ./setup.sh
EXE=`basename $PWD` 
#mpirun -np 4 
${TEST_RUN} ./$EXE
