#!/bin/bash

clang ../basic.c -emit-llvm -o basic.ll -c -S
clang ../basic2.c -emit-llvm -o basic2.ll -c -S
clang ../pi.c -emit-llvm -o pi.ll -c -S
clang ../pi2.c -emit-llvm -o pi2.ll -c -S
