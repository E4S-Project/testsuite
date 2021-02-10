#!/bin/bash

. ../testfunctions.sh

SOURCES="householder3.c matmul.c Q.c R.c"

# Generate .symbol database if needed
symbols::analysis "$SOURCES" "C"

