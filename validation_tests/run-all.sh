#!/bin/bash

function e() {
  ./"${1}" > /dev/null 2>&1
  return $?
}

for d in "hypre" "sundials" "strumpack" "trilinos" "superlu-dist" "superlu"
do
  passed=0
  (cd "${d}" && e "clean.sh" && e "compile.sh" && e "run.sh") && passed=1
  if [[ "${passed}" -eq 1 ]]; then
    printf "${d} passed!\n"
  else
    printf "${d} FAILED**\n"
  fi
done
