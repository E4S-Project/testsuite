#!/bin/bash

module load chill/git
module load isl
module load llvm/kitsune/9.0   # the LLVM installation Chill was compiled against

ulimit -s unlimited
