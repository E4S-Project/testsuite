#!/bin/bash
. ./setup.sh
set -e
set -x
#nekbmpi fdlf 4
echo fdlf        >  SESSION.NAME
echo `pwd`'/' >>  SESSION.NAME
${TEST_RUN} ./nek5000 > fdlf.out
tail fdlf.out


visnek fdlf
#${TEST_RUN} ./nek5000 fdlf
 
