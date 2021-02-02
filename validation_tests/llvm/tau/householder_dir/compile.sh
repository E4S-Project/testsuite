#!/bin/bash

. ./setup.sh

test::compile ./functions_C_files2.txt householder "$SOURCES"
test::compile ./functions_C_files3.txt householderfileWC "$SOURCES"
test::compile ./functions_CXX_hh_files.txt householdercxx "$SOURCES"
