#!/bin/bash

LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH ./callfunctions_c > tmpc
LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH ./callfunctions2_c > tmpc2
LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH ./callfunctions_f > tmpf

TOTO=`diff tmpc tmpf`
echo -n "Comparing C and Fortran calls:  "
if [ -z $TOTO ]; then
    echo "[PASS]"
else
    echo "[FAIL]"    
fi
TOTO=`diff tmpc2 tmpc`
echo -n "Comparing C with and without _: "
if [ -z $TOTO ]; then
    echo "[PASS]"
else
    echo "[FAIL]"    
fi
rm tmpc tmpc2 tmpf
