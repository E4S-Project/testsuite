#!/bin/bash

. ./setup.sh

ulimit -s unlimited
cd lapack && make blaslib lapacklib
cd ..

cd gfortran.dg && if [ ! -d build ] ; then mkdir build ; fi && cd build && cmake .. -DFC=f18
cd ../..

