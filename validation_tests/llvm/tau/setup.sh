#!/bin/bash

export LLVM_DIR=`which clang | awk -F"bin" {'print $1'}`
