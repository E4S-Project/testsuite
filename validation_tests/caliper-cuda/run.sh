#!/bin/bash -e

TMPFILE=$(mktemp ./tmp.XXXXXXX)

. ./setup.sh
set -x

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CUDA_ROOT/extras/CUPTI/lib64

eval ./caliper -P runtime-report,profile.cuda

if [ $? -ne 0 ]
then
  echo -e "${BRED}[FAILED]${NC}"
  exit 1
else
  echo -e "${BGREEN}[PASSED]${NC}"
fi
