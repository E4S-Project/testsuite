#!/bin/bash

. ./setup.sh


compiletest "./functions_C_files2.txt" "householder" "householder.cpp R.cpp Q.cpp matmul.cpp"
compiletest "./functions_C_files3.txt" "householderfileWC" "householder.cpp R.cpp Q.cpp matmul.cpp"
compiletest "./functions_CXX_hh_files.txt" "householdercxx" "householder.cpp R.cpp Q.cpp matmul.cpp"
