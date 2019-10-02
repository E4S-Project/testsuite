#!/bin/bash

. ./setup.sh
export TRILINOS=`spack location -i trilinos`

function e() {
  output=/dev/null
  if [[ $# -eq 2 ]]; then
    output="${2}"
  fi
  ./"${1}" >> "${output}" 2>&1
  return $?
}

cwd=`pwd`
log="${cwd}/log"
touch "$log"

for td in `ls`
do
  if [[ -d "${td}" ]]; then
    p=0
    a=0
    cd "${td}" && a=1 && e "clean.sh" "${log}" && e "compile.sh" "${log}" && e "run.sh" "${log}" && p=1
    if [[ ${a} -eq 1 ]]; then cd ..; fi
    if [[ ${p} -eq 1 ]]; then
      printf "  ${td} passed!\n"
    else
      printf "  ${td} FAILED**!\n"
    fi
  fi
done 
