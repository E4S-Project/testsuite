#!/bin/bash

make clean

rm gang parallel inout jacobi inout_data jacobi_data
for TARGET in host x86_64  nvptx64 ; do
    rm -Rf jacobi_$TARGET 
done
