#!/bin/bash

. ./setup.sh
#mpicc
${TEST_CC_MPI} -o heatdis_mem heatdis_mem.c -lveloc-client -lm
