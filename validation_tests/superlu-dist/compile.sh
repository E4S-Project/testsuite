#!/bin/bash

for i in `cat testdirs.txt`; do
  cd "${i}" && ./compile.sh && cd .. || exit 1
done
