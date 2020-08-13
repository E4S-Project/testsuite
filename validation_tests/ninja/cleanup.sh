#!/bin/bash

cd basic
./cleanup.sh
cd ..

cd library
./cleanup.sh
cd ..

cd generators/cmake
./cleanup.sh
cd ../..

cd generators/python
./cleanup.sh
cd ../..
