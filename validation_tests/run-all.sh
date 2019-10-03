#!/bin/bash

wd=`pwd`
date=`date '+%y%m%d.%H%M%S'`
logf="${wd}/test-output.${date}.log"

function e() {
  ./"${1}" >> "${logf}" 2>&1
  return $?
}

for d in `cat testdirs.txt`
do
  passed=0
  (cd "${d}" && e "clean.sh" && e "compile.sh" && e "run.sh") && passed=1
  if [[ "${passed}" -eq 1 ]]; then
    printf "${d} passed!\n"
  else
    printf "${d} FAILED**\n"
  fi
done

printf "Combined test output written to: ${logf}\n\n"
