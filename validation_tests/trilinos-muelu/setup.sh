#!/bin/bash
. ../../setup.sh

THISDIR=`basename "$PWD"`
USECUDA=0
#if [  "$THISDIR" = "trilinos-cuda" ];then
        spackLoadUnique trilinos+cuda  cuda_arch=70
	USECUDA=1
#elif [  "$THISDIR" = "trilinos-rocm" ];then
#        spackLoadUnique trilinos+rocm

#else
#	spackLoadUnique trilinos~cuda
#fi

if ! command -v cmake >/dev/null 2>&1 ; then
  spackLoadUniqueNoX cmake@3.17.1:
fi
