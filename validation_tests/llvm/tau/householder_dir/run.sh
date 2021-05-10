#!/bin/bash

. ./setup.sh

#export QUIET_TEST_OUTPUT=1

test::run "functions_C_files2.txt" "householder"
test::run "functions_C_files3.txt" "householderfileWC"
test::run "functions_CXX_hh_files.txt" "householdercxx"

#unset QUIET_TEST_OUTPUT
