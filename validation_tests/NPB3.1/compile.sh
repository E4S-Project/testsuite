#!/bin/bash
. ../../setup.sh
spack load mpich

mkdir -p bin
make suite
