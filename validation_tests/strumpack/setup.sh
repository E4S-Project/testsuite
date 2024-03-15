#!/bin/bash

. ../../setup.sh

if ! command -v cmake >/dev/null 2>&1 ; then
  spackLoadUniqueNoX cmake
fi

spackLoadUnique strumpack~rocm
