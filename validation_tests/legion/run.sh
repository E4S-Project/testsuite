#!/bin/bash -e


. ./setup.sh
set -x
TMPFILE=$(mktemp ./tmp.XXXXXXX)

cd build

 ./legion > ${TMPFILE}

if [ $? -ne 0 ]
then
  echo -e "${BRED}[FAILED]${NC}"
  exit 1
else
  echo -e "${BGREEN}[PASSED]${NC}"
fi
