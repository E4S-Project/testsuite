#!/bin/bash

. ./setup.sh

./conduit_example
#mpiexec -n 4 
eval $TEST_RUN ./conduit_mpi_example
