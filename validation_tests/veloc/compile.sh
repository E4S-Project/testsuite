#!/bin/bash

. ./setup.sh
#mpicc
${TEST_CC_MPI} -o heatdis_fault heatdis_fault.cpp -I${VELOC_ROOT} -L${VELOC_LIB_PATH} -lveloc-client -lm
