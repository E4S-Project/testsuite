#!/bin/bash 
. ./setup.sh
set -e
set -x
cwd=`pwd`
for i in `cat testdirs.txt`; do
  cd "${cwd}"
  echo "BUILDING TEST: ${i}" 
  cd "${i}" && ./compile.sh || exit 1
done
