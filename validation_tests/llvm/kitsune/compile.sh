#!/bin/bash

. ./setup.sh

echo "[Basic sanity checks]"
clang -ftapir=openmp -fopenmp -o basic basic.c
clang -ftapir=openmp -fopenmp -o basic2 basic2.c
