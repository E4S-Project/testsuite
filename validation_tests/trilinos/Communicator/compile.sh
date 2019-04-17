#!/bin/bash
. ../setup.sh
export TRILINOS=`spack location -i trilinos`

make clean
make
