#!/bin/bash

LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH ./callfunctions_c > tmpc
LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH ./callfunctions_f > tmpf

TOTO=`diff tmpc tmpf`
if [ -z $TOTO ]; then
    echo "[PASS]"
else
    echo "[FAIL]"    
fi
rm tmpc tmpf
