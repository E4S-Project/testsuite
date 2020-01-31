#!/bin/bash -ex

. ../setup.sh
. ./setup.sh

cwd=`pwd`
for i in `cat testdirs.txt`; do
  cd "${cwd}"
  cd "${i}" && ./run.sh || exit 1
done
