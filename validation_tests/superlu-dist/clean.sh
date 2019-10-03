#!/bin/bash
for i in `cat testdirs.txt`; do
  (cd "${i}" && ./clean.sh)
done
