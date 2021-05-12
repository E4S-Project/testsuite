#!/bin/bash

. ./setup.sh

$TEST_CXX_MPI column_wise.cpp -o column_wise -I${PARALLEL_NETCDF_ROOT}/include -L${PARALLEL_NETCDF_LIB_PATH} -lpnetcdf
