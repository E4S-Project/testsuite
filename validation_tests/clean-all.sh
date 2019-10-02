#!/bin/bash

function e() {
  ./"${1}" > /dev/null 2>&1
  return $?
}

for d in "hypre" "sundials" "strumpack" "trilinos" "superlu-dist" "superlu"
do
  (cd "${d}" && e "clean.sh")
done
