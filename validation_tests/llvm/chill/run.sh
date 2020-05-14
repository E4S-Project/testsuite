#!/bin/bash

# Verification: can we start CHiLL

for EXEC in "chill" "cuda-chill" ; do
    echo "exit()" | $EXEC > /dev/null
    RET=$?
    echo -n "Start :" $EXEC "     "
    if [ $RET == 0 ] ; then
	echo "[PASS]"
    else
	echo "[FAIL]"
    fi
done
    
# Run a simple example

chill simple.py &> /dev/null
RET=$?
echo -n "Simple test :           "
if [ $RET == 0 ] ; then
    echo "[PASS]"
else
    echo "[FAIL]"
fi
# It must have produced an output file
echo -n "Output file present:    "
if test -f "src/singleloop_modified.c"; then   
    echo "[PASS]"
else
    echo "[FAIL]"
fi
# Does it give the same result
cd testfiles
echo -n "Produced code gives the same result:   "
gcc -o single_main single_main.c ../src/singleloop_modified.c
./single_main > tmp2
gcc -o single_main single_main.c ../src/singleloop.c
./single_main > tmp
TOTO=`diff tmp tmp2`
if [ -z $TOTO ]; then
    echo "[PASS]"
else
    echo "[FAIL]"    
fi
rm tmp tmp2
cd ..

# When we try to load a file that does not exist, CHiLL segfaults
chill nofile.py &> /dev/null
RET=$?
echo -n "Segmentation fault when trying to load a non-existent file:   "
if [ $RET == 139 ] ; then
    echo "[PASS]"
else
    echo "[FAIL]"
fi
