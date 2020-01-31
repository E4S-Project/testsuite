#!/bin/bash -ex
. ./setup.sh
cwd=`pwd`
for i in `cat testdirs.txt`; do
  cd "${cwd}"
  echo "BUILDING TEST: ${i}" 
  cd "${i}" && ./compile.sh || exit 1
done
