#!/bin/bash

make clean
module unload llvm/pragma/loop/git

rm -f matmul_unchanged    permutation    reverse tile    unrolling crout matmul
