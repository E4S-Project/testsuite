#!/bin/bash

. ./setup.sh
export OMP_PROC_BIND=spread
install/build/my_program/myTest
install/build/my_program/KokkosSparse_wiki_spadd
#./blockjacobi -Task 1 -TeamSize 32
