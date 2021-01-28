#!/bin/bash

#export LLVM_DIR=`which clang | awk -F"bin" {'print $1'}`

### modules for illyad
module load llvm/inuse
module load gcc
module load tau2/x86_64

. ../testfunctions.sh


### modules for gorgon
#module load llvm
#module load tau2/current
