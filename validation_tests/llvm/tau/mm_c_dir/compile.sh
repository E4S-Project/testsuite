#!/bin/bash

. ./setup.sh

test::compile "./functions_C_mm_files.txt" "mm_c" "matmult.c matmult_initialize.c" "C"
