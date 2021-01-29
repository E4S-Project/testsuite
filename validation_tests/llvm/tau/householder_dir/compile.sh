#!/bin/bash

. ./setup.sh

compiletest ./functions_C_files2.txt householder "$SOURCES"
compiletest ./functions_C_files3.txt householderfileWC "$SOURCES"
compiletest ./functions_CXX_hh_files.txt householdercxx "$SOURCES"
