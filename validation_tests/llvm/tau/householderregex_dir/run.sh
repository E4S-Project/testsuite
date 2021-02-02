#!/bin/bash

. ./setup.sh

test::run "functions_CXX_hh_regex.txt" "householder"
test::run "functions_CXX_hh_regex_exclude.txt" "householder_exclude" 
