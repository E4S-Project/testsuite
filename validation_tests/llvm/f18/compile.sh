#!/bin/bash

ulimit -s unlimited
cd lapack && make blaslib lapacklib blas_testing lapack_testing 

