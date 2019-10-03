#!/bin/bash

for i in `cat testdirs.txt`; do
  cd "${i}" && ./run.sh && cd .. || exit 1
done
