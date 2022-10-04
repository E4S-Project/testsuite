#!/bin/bash
. ../../setup.sh
spackLoadUnique ninja
spackLoadUnique llvm
if ! command -v cmake >/dev/null 2>&1 ; then
  spackLoadUnique cmake
fi

