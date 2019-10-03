#!/bin/bash
export TRILINOS=`spack location -i trilinos`
for d in `cat testdirs.txt`; do
  if [[ -d "${d}" ]]; then (cd "${d}" && ./clean.sh) ; fi
done

rm -f log
