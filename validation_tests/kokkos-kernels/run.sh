#!/bin/bash

. ./setup.sh
export OMP_PROC_BIND=spread
./blockjacobi -Task 1 -TeamSize 32
