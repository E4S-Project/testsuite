#!/bin/bash -ex

TMPFILE=$(mktemp -d ./tmpdir.XXXXXXX)

. ./setup.sh

cd $TMPFILE

flit init --litmus-test
make gt
./gtrun DoMatrixMultSanity


if [ $? -ne 0 ]
then
  echo -e "${BRED}[FAILED]${NC}"
  exit 1
else
  echo -e "${BGREEN}[PASSED]${NC}"
fi
