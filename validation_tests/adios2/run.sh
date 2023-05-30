#!/bin/bash

. ./setup.sh

#mpiexec -n 4 
eval $TEST_RUN  ./helloBPTimeWriter 
