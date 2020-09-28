#!/bin/bash

#cd lapack && make clean

for d in libraries   modules  simple ; do
    cd $d
    ./clean.sh
    cd ..
done

rm -f version
