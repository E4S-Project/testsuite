#!/bin/bash

module load llvm/sollve/git python/3.6.8
if [ $e4s_print_color = true -a  -n "$TERM" ];
then
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'

BRED='\033[1;31m'
BGREEN='\033[1;32m'
BBLUE='\033[1;34m'

NC='\033[0m'
fi
