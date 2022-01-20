#!/bin/bash

. ../../setup.sh

if ! command -v pkg-config >/dev/null 2>&1 ; then
  spackLoadUnique pkg-config
fi

spackLoadUnique gasnet

export PKG_CONFIG_PATH=$GASNET_ROOT/lib/pkgconfig/:$PKG_CONFIG_PATH
NP=4
