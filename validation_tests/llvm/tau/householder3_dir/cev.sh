#!/bin/bash

. ./setup.sh

InputFile=$1

cevtest $InputFile householder3 "householder3.c  matmul.c  Q.c  R.c" "C"

