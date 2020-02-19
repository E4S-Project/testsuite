#!/bin/bash

echo "[Basic sanity checks]"
clang -fopenmp -o basic basic.c
clang -fopenmp -o basic2 basic2.c
