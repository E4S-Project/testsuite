#!/bin/bash

LD_PRELOAD=./profiling.so ./basic2
LD_PRELOAD=./profiling.so ./jacobi_off_x86
LD_PRELOAD=./profiling.so ../householder 64 64


