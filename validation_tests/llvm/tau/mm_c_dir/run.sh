#!/bin/bash

. ./setup.sh

export QUIET_TEST_OUTPUT=1

test::run "functions_C_mm_files.txt" "mm_c" "C"

unset QUIET_TEST_OUTPUT
