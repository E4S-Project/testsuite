#!/bin/bash
. ../../setup.sh
spackLoadUnique openblas threads=openmp
spackLoadUnique strumpack
spackLoadUnique openblas threads=openmp

