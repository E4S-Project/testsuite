#!/bin/bash

. ./setup.sh

InputFile=$1

cevtest $InputFile householder "householder.cpp R.cpp Q.cpp matmul.cpp"
