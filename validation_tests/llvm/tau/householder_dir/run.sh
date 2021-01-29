#!/bin/bash

. ./setup.sh

#runtest "functions_C_files2.txt" "householder"
#runtest "functions_C_files3.txt" "householderfileWC"
runtest "functions_CXX_hh_files.txt" "householdercxx"
