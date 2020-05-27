#!/bin/bash

TESTLIST="01_open_close  02_put_get  03_barrier  04_delete  05_fence  06_signal  07_consistency  08_protect  09_cache  10_checkpoint  11_restart  12_free "
NP=4

for d in $TESTLIST; do 
    cd $d
    echo -n "Papyrus test $d:   "
    mpiexec -n $NP ./test${d} 2>&1 > toto
    # test completion and success
    ERROR=`grep "\[E\] " toto`
    if [[ ! -z $ERROR ]] ; then
	echo "[FAIL]"
    else
	echo "[PASS]"
    fi
    rm toto
    cd ..
done
