#!/bin/bash

. ../../setup.sh
spackLoadUnique gasnet

export PKG_CONFIG_PATH=$GASNET_ROOT/lib/pkgconfig/:$PKG_CONFIG_PATH
NP=4
