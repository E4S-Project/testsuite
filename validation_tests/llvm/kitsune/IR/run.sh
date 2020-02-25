#!/bin/bash

D=`grep "detach" *.ll | wc -l` 
R=`grep "reattach" *.ll | wc -l` 
S=`grep "sync" *.ll | wc -l` 

if [ $(($R+$D+$S)) -eq 0 ] ; then
    echo "[FAILED] Did not find the MLIR extensions"
else
    echo "[PASS] Found the MLIR extensions"
fi

