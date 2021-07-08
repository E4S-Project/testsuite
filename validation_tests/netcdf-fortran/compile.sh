#!/bin/bash

. ./setup.sh

$TEST_FTN_MPI simple_xy_wr.f90 -o simple_xy_wr -I${NETCDF_FORTRAN_ROOT}/include -L${NETCDF_FORTRAN_ROOT}/lib -Wl,-rpath,${NETCDF_FORTRAN_ROOT}/lib  -lnetcdff
