#!/bin/bash

. ./setup.sh

mpicxx -o slate04_blas slate04_blas.cc -lslate -lcudart -lcublas -fopenmp

