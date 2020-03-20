#!/bin/bash

clang ../basic.c -ftapir=openmp -emit-llvm -o basic.ll -c -S
clang ../basic2.c -ftapir=openmp -emit-llvm -o basic2.ll -c -S
clang ../pi.c -ftapir=openmp -emit-llvm -o pi.ll -c -S
clang ../pi2.c -ftapir=openmp -emit-llvm -o pi2.ll -c -S
