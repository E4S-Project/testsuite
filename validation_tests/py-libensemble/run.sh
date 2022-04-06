#!/bin/bash -e
. ./setup.sh
set -x
TMPFILE=$(mktemp ./tmp.XXXXXXX)
if [ $e4s_print_color = true -a  -n "$TERM" ];
then
BRED='\033[1;31m'
BGREEN='\033[1;32m'

NC='\033[0m'
fi
python calling_script.py > $TMPFILE 

if [ $? -ne 0 ]
then
  echo -e "${BRED}[FAILED]${NC}"
  exit 1
else
  echo -e "${BGREEN}[PASSED]${NC}"
fi

#echo $(grep -E "Total" $TMPFILE)
#
#if [ $(grep "PASSED" $TMPFILE | wc -l) -ne 0 ]; then
#    echo -e "${BGREEN}[PASSED]${NC}"
#else
#    echo -e "${BRED}[FAILED]${NC}"
#fi
#
