#!/bin/bash

. ./setup.sh
#mpicc
${TEST_CC_MPI} -o heatdis_mem heatdis_mem.c -I${VELOC_ROOT}/include -L${VELOC_LIB_PATH} -lveloc-client -lm
