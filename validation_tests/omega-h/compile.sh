#!/bin/bash
. ./setup.sh
#mpicxx 
eval $TEST_CXX -o castle main.cpp -lomega_h
