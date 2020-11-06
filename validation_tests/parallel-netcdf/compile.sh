#!/bin/bash

. ./setup.sh

$TEST_CXX_MPI column_wise.cpp -o column_wise -lpnetcdf
