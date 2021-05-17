#!/bin/bash -e
#-ex
. ./setup.sh
set -x
cd ./build
export SCR_USER_NAME=`whoami`
#export SCR_PREFIX=/tmp/$SCR_USER_NAME/scr.defjobid
CLEANTMP="rm -rf /tmp/$SCR_USER_NAME/scr.defjobid; rm -rf ./output* ./rank*  ./ckpt.*;rm -rf ./*.ckpt; rm -rf ./timestep.*; rm -rf ./.scr"
eval $CLEANTMP
#mpirun -np 4 
eval $TEST_RUN ./test_api 
eval $CLEANTMP
#mpirun -np 4 
eval $TEST_RUN ./test_api_multiple  
eval $CLEANTMP
#mpirun -np 4 
eval $TEST_RUN ./test_ckpt          
eval $CLEANTMP
#mpirun -np 4 
eval $TEST_RUN ./test_ckpt_F
eval $CLEANTMP
cd -
