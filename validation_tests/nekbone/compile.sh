#!/bin/bash
. ./setup.sh
set -x
set -e

cp -r $NEKBONE_ROOT/bin/Nekbone/test/example1 .
cd example1
cp $NEKBONE_ROOT/bin/makenek .

sed -i '/^F77=/s|"$| -fallow-argument-mismatch"|' "./makenek"

./makenek
