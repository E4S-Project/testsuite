#!/bin/bash

. ./setup.sh

#export QUIET_TEST_OUTPUT=1

test::run "functions_C_files.txt" "householder3" "C"

#unset QUIET_TEST_OUTPUT
