#!/bin/bash

TMPFILE=$(mktemp ./tmp.XXXXXXX)

. ./setup.sh

eval ./caliper

CALI_CONFIG_PROFILE=runtime-report ./caliper

if [ $? -ne 0 ]
then
  echo -e "${BRED}[FAILED]${NC}"
  exit 1
else
  echo -e "${BGREEN}[PASSED]${NC}"
fi
