#!/bin/bash -e

source setup.sh

spackLoadUnique cmake

cmake -S . -B build
cmake --build build
