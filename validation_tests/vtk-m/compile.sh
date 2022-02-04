#!/bin/bash -e

source setup.sh

spackLoadUnique cmake

cmake -S src -B build
cmake --build build
