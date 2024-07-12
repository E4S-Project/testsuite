#!/bin/bash
. ./setup.sh
set -x
set -e
cp -r $NVHPC_ROOT/Linux_x86_64/2024/examples/MPI/* .

        cd samples/buflimit; make build; 
cd -
        cd samples/copytest; make build; 
cd -

        cd samples/goptest; make build;
cd -

        cd samples/mpihello; make build; 
cd -

        cd samples/mpptest; make build; 
cd -

        cd scalapack; make build; 


