#!/bin/bash

rm -f profile.*
rm -f basic basic2 jacobi profiling.so

if [ `arch` == "x86_64" ]; then
    ARCHITECTURES=(nvptx64-nvidia-cuda x86_64-unknown-linux-gnu)
fi
if [ `arch` == "ppc64le" ]; then
    ARCHITECTURES=(nvptx64-nvidia-cuda powerpc64le-unknown-linux-gnu)
fi
for TARGET in ${ARCHITECTURES[@]} ; do
    rm -f jacobi_off_$TARGET 
done
