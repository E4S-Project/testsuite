#!/bin/bash 
#-ex
. ./setup.sh
export SCR_USER_NAME=root
#mpirun -np 4 
eval $TEST_RUN ./test_api  
#mpirun -np 4 
eval $TEST_RUN ./test_api_multiple  
#mpirun -np 4 
eval $TEST_RUN ./test_ckpt_C          
#mpirun -np 4 
eval $TEST_RUN ./test_ckpt_F


