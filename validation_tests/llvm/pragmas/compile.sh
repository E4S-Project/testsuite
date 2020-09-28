#!/bin/bash

. ./setup.sh
clang -O3 -o blocklu blocklu.c -lrt -lm


