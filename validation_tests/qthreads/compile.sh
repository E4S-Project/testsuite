#!/bin/bash
. ../../setup.sh
spack load qthreads

gcc -lqthread -I. ./hello_world_multi.c



