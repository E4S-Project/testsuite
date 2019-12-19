#!/bin/bash

echo "Compiling with gfortran"
make FC=gfortran
make clean

echo "Compiling with f18"
make FC=f18
make clean

