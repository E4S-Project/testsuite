#!/bin/bash
if [ -z "$srcdir" ]; then export srcdir=`dirname $0`; fi

# Determine the proper "-k" format options for `ncgen`.
if [ $# -lt 1 ]; then
    echo "Usage : $0 <test-id>"
fi
ID=$1
IN=$srcdir/test01a.cdl
OUT=test01f.$ID.nc
CMD="ncgen -k nc4 -o $OUT $IN"
eval $CMD

if test "$?" = "0"; then
    echo nc3 nc4
    exit
fi

CMD2="ncgen -k hdf5 -o $OUT $IN"
eval $CMD2

if test "$?" = "0"; then
    echo 1 3
    exit
fi

echo UNKNOWN UNKNOWN
exit 1