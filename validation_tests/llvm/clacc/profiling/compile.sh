#!/bin/bash

clang -fPIC -shared -fopenacc -o profiling.so profiling.c
