#!/bin/bash

. ./setup.sh

export QUIET_TEST_OUTPUT=1

runtest "functions_C_files2.txt" "householder"
runtest "functions_C_files3.txt" "householderfileWC"
runtest "functions_CXX_hh_files.txt" "householdercxx"

unset QUIET_TEST_OUTPUT
