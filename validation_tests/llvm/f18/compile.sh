#!/bin/bash

. ./setup.sh

ulimit -s unlimited
cd lapack && make blaslib lapacklib
cd ..

cd fgortran.dg && mkdir -f build && cd build && cmake .. -DFC=f18
cd ../..

