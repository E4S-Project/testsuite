#!/bin/bash

./callfunctions_c > tmpc
./callfunctions_f > tmpf

TOTO=`diff tmpc tmpf`
if [ -z $TOTO ]; then
    echo "[PASS]"
else
    echo "[FAIL]"    
fi
rm tmpc tmpf
