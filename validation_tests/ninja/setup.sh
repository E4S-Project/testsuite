#!/bin/bash
. ../../setup.sh
spackLoadUnique ninja
if ! command -v cmake >/dev/null 2>&1 ; then
  spackLoadUnique cmake
fi

