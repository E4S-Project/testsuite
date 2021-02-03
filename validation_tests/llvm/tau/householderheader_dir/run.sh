#!/bin/bash

. ./setup.sh

export QUIET_TEST_OUTPUT=1

test::run "functions_CXX_header_implemented.txt" "householderheader"

unset QUIET_TEST_OUTPUT
