#!/bin/bash -ex
. ./setup.sh
cwd=`pwd`
for i in `cat testdirs.txt`; do
  cd "${cwd}"
  cd "${i}" && ./compile.sh || exit 1
done
