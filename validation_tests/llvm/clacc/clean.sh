#!/bin/bash

make clean

rm -f gang kernel parallel inout jacobi inout_data jacobi_data

if [ `arch` == "ppc64le" ]; then
    ARCHITECTURES=(nvptx64-nvidia-cuda powerpc64le-unknown-linux-gnu)
fi
if [ `arch` == "x86_64" ]; then
    ARCHITECTURES=(nvptx64-nvidia-cuda x86_64-unknown-linux-gnu)
fi

for TARGET in ${ARCHITECTURES[@]} ; do
    rm -Rf jacobi_$TARGET 
done
