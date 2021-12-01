#!/bin/bash
source ./setup.sh
export TRILINOS=$TRILINOS_ROOT #`spack location -i trilinos`
for d in `cat testdirs.txt`; do
  if [[ -d "${d}" ]]; then (cd "${d}" && ./clean.sh) ; fi
done

rm -f log
