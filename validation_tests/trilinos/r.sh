#!/bin/bash

. ./setup.sh

cwd=`pwd`
log="${cwd}/log"
touch "$log"

for td in `ls`
do
  if [[ -d "${td}" ]]; then
    p=0
    a=0
    cd "${td}" && a=1 && ./clean.sh >>"${log}" 2>&1 && ./compile.sh >>"${log}" 2>&1 && ./run.sh >>"${log}" 2>&1 && p=1
    if [[ ${a} -eq 1 ]]; then cd ..; fi
    if [[ ${p} -eq 1 ]]; then
      printf "${td} passed!\n"
    else
      printf "${td} FAILED**!\n"
    fi
  fi
done 
