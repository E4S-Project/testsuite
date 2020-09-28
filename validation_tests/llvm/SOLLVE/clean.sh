#!/bin/bash

rm -f basic basic2
cd pragmas
./clean.sh
cd ..

module unload llvm/sollve/git
