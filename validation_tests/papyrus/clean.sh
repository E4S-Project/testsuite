#!/bin/bash

TESTLIST="01_open_close  02_put_get  03_barrier  04_delete  05_fence  06_signal  07_consistency  08_protect  09_cache  10_checkpoint  11_restart  12_free "

for d in $TESTLIST; do
    rm -f $d/test${d}
done
