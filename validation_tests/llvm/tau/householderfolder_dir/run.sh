#!/bin/bash

. ./setup.sh

export QUIET_TEST_OUTPUT=1

test::run "functions_CXX_folder.txt" "householder_folder"

unset QUIET_TEST_OUTPUT
