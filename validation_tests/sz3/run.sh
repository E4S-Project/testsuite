#!/bin/bash
. ./setup.sh
set -e
set -x
sz3 -f -i $SZ3_ROOT/share/SZ3/testfloat_8_8_128.dat -z ./testfloat_8_8_128.dat.sz -3 8 8 128 -M ABS 1e-3
sz3 -f -z ./testfloat_8_8_128.dat.sz -o ./testfloat_8_8_128.dat.sz.out -3 8 8 128 -M REL 1e-3

