#!/bin/bash

chill matmul.py
chill applyR.py
chill applyQ.py

clang -O3 -Wall -o hh src/hh_main.c src/matmul.c src/applyQ.c src/applyR.c src/householder2.c -lm

clang -O3 -Wall -o hh_modified src/hh_main.c src/matmul_modified.c src/applyQ_modified.c src/applyR_modified.c src/householder2.c -lm
