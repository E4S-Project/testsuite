#!/bin/bash
. ./setup.sh
set -x
set -e

cp -r $NEKBONE_ROOT/bin/Nekbone/test/example1 .
cd example1
cp $NEKBONE_ROOT/bin/makenek .

# GCC >= 10 needs -fallow-argument-mismatch; GCC 9 rejects it (and doesn't need it)
if echo "      end" | mpif77 -fallow-argument-mismatch -x f77 -c -o /dev/null - 2>/dev/null; then
    sed -i '/^F77=/s|"$| -fallow-argument-mismatch"|' ./makenek
fi

./makenek
