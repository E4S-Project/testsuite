#!/bin/bash

. ./setup.sh

InputFile=$1
cevtest $InputFile "mm_c" "matmult.c matmult_initialize.c" "C"

