#!/bin/bash

./setup.sh
#./householder 1024 1024

./parallel
./jacobi
./jacobi_data

NB=4
STARTED=`./gang $NB | grep "toto" | wc -l`
if [ $NB == $STARTED ]; then
    echo "Gang: [PASS]"
else
    echo "Gang:[FAIL] launched", $STARTED, " instead of ", $NB, "requested"
fi


./inout
if [ $? == 0 ] ; then
    echo "Data in/out: [PASS]"
else
    echo "Data in/out: [FAIL]"
fi

./inout_data
if [ $? == 0 ] ; then
    echo "Data in/out (2): [PASS]"
else
    echo "Data in/out (2): [FAIL]"
fi
