#!/bin/bash
spack load mpich

mkdir -p bin
make suite
