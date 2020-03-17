#!/bin/bash

./setup.sh
#./householder 1024 1024

./parallel

NB=4
STARTED=`./gang $NB | grep "toto" | wc -l`
if [ $NB == $STARTED ]; then
    echo "Gang: [PASS]"
else
    echo "Gang:[FAIL] launched", $STARTED, " instead of ", $NB, "requested"
fi


