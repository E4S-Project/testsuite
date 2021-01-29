#!/bin/bash

. ./setup.sh


compiletest "./functions_C_files.txt" "householder3" "matmul.c householder3.c Q.c R.c" "C" 
