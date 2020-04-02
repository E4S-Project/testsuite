#!/bin/bash

clang -fopenacc -o basic basic.c
clang -fopenacc -o basic2 basic2.c
clang -fopenacc -o jacobi jacobi.c

clang -fopenacc -fopenmp-targets=x86_64  -o jacobi_off_x86 jacobi.c

clang -fPIC -shared -fopenacc -o profiling.so profiling.c

