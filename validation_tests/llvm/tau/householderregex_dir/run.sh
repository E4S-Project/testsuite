#!/bin/bash

. ./setup.sh

export QUIET_TEST_OUTPUT=1

test::run "functions_CXX_hh_regex.txt" "householder"
test::run "functions_CXX_hh_regex_exclude.txt" "householder_exclude" 

unset QUIET_TEST_OUTPUT
