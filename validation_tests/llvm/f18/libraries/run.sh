#!/bin/bash

# Dynamic libraries

echo "[Dynamic libraries]"

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

# Static libraries

echo "[Static libraries]"

./callfunctions_c_static > tmpc
./callfunctions2_c_static > tmpc2
./callfunctions_f_static> tmpf

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

