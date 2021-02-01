#!/bin/bash

. ../testfunctions.sh

SOURCES="matmult.c matmult_initialize.c"

# Generate .symbol database if needed
symbols::analysis "$SOURCES"

