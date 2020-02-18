#!/bin/bash

LD_PRELOAD=./profiling.so ./basic2
LD_PRELOAD=./profiling.so ../householder 64 64


