#!/bin/bash
. ./setup.sh
set -e
set -x

if [ -d "masterclass-21-1" ]; then
echo "Masterclass data already present"
else
git clone https://github.com/plumed/masterclass-21-1.git

fi

${TEST_RUN} plumed driver --plumed ./masterclass-21-1/work/plumed_ex1.dat --mf_xtc ./masterclass-21-1/data/5-HT1B.xtc
cat ./COLVAR
