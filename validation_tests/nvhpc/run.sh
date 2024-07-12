#!/bin/bash
. ./setup.sh
set -x
set -e
export NPROCS=4
        cd samples/buflimit;  make run; make clean
cd -
        cd samples/copytest;  make NPROCS=$NPROCS run; 
cd -

        cd samples/goptest;  make NPROCS=$NPROCS run; 
cd -

        cd samples/mpihello; make NPROCS=$NPROCS run; 
cd -

        cd samples/mpptest;  make NPROCS=$NPROCS run; 
cd -

        cd scalapack;  make NPROCS=$NPROCS run; 
 
