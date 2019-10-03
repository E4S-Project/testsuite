#!/bin/bash

function e() {
  ./"${1}" > /dev/null 2>&1
  return $?
}

for d in `cat testdirs.txt`
do
  (cd "${d}" && e "clean.sh")
done
