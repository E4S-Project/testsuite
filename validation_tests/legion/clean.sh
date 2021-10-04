#!/bin/bash 
cd build
rm -rf Makefile CMakeCache* cmake* CMakeFiles legion tmp.*
cd ../
rm -f *.log tmp.*
