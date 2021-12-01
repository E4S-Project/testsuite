#!/bin/bash

. ../setup.sh
. ./setup.sh
set -e 
cwd=`pwd`
for i in `cat testdirs.txt`; do
  cd "${cwd}"
  cd "${i}" && ./run.sh || exit 1
done
