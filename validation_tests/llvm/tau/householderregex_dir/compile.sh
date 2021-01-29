#!/bin/bash

. ./setup.sh

SOURCES="householder.cpp R.cpp Q.cpp matmul.cpp"

compiletest "functions_CXX_hh_regex.txt" "householder" "$SOURCES"

