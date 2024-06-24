#!/bin/bash
. ./setup.sh
set -x
set -e
for testexe in *.exe; do
   ${TEST_RUN} ./"$testexe"
done

 
