#!/bin/bash

. ../testfunctions.sh

SOURCES="householder.cpp R/R.cpp Q.cpp matmul.cpp"

# Generate .symbol database if needed
symbols::analysis "$SOURCES"
