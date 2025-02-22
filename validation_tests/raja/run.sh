#!/bin/bash 
#. ../../setup.sh
. ./setup.sh
set -e
set -x
cd build/bin
for file in *_solution; do
    if [[ -x "$file" ]]; then
        ./"$file"
    fi
done
