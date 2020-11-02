#!/bin/bash -ex
cd build
rm -rf CMakeCache* cmake* CMakeFiles legion tmp.*
cd ../
rm *.log tmp.*
