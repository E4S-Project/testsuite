#!/bin/bash

clang -fopenacc -o basic basic.c
clang -fopenacc -o basic2 basic2.c
clang -fopenacc -o jacobi jacobi.c

if [ `arch` == "ppc64le" ]; then
    ARCHITECTURES=(nvptx64-nvidia-cuda powerpc64le-unknown-linux-gnu)
fi
if [ `arch` == "x86_64" ]; then
    ARCHITECTURES=(nvptx64-nvidia-cuda x86_64-unknown-linux-gnu)
fi
for TARGET in ${ARCHITECTURES[@]} ; do
    clang -O3 -fopenacc -fopenmp-targets=$TARGET -o jacobi_off_$TARGET jacobi.c
done

clang -fPIC -shared -fopenacc -o profiling.so profiling.c

