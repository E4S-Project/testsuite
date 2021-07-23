#!/bin/bash

. ./setup.sh

$TEST_CXX_MPI simple_xy_nc4_wr.c -o simple_xy_nc4_wr -I${NETCDF_C_ROOT}/include -L${NETCDF_C_ROOT}/lib -Wl,-rpath,${NETCDF_C_ROOT}/lib  -lnetcdf
