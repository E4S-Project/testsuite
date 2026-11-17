#!/bin/bash

set -x

export ABI_PSPDIR="$PWD/Pspdir"

${TEST_RUN} abinit tbase1_1.abi
