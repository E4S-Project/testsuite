#!/bin/bash

export PAPYRUSDIR=/tmp/root/spack-stage/spack-stage-papyrus-1.0.0-j7sq7ppf24fu4q6mbgxoz24x434copzw/spack-src/install
export LIBPAPYRUS=${PAPYRUSDIR}/lib/libpapyruskv.a

TESTLIST="01_open_close  02_put_get  03_barrier  04_delete  05_fence  06_signal  07_consistency  08_protect  09_cache  10_checkpoint  11_restart  12_free "
# The last one, 13_upc, is using UPC.

for d in $TESTLIST; do 
    cd $d
    mpicxx -I${PAPYRUSDIR}/include -g -o test${d} test${d}.c $LIBPAPYRUS -lpthread -lm
    cd ..
done
