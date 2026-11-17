#!/bin/bash

set -x

export ABI_PSPDIR="$PWD/Pspdir"

mpirun -np 4 abinit tbase1_1.abi
