#!/bin/bash
rm -f *.o  profile*
export TRILINOS=`spack location -i trilinos`
make clean

