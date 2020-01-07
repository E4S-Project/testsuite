#!/bin/bash
export PRAGMAS=$HOME/x86/pragmas/llvm-project/
export LOOP=$PRAGMAS/install-pragma-clang-loop/
export TRANS=$PRAGMAS/install-pragma-clang-transform

export PATH=$LOOP/bin:$TRANS/bin:$PATH
export LD_LIBRARY_PATH=$LOOP/lib:$TRANS/lib:$LD_LIBRARY_PATH

