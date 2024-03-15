#!/bin/bash
. ../../setup.sh

THISDIR=`basename "$PWD"`
if [  "$THISDIR" = "trilinos-rocm" ];then
        spackLoadUnique trilinos+rocm  amdgpu_target=gfx908
	USEROCM=1
elif [  "$THISDIR" = "trilinos-cuda" ];then
        spackLoadUnique trilinos+cuda
	USECUDA=1
else
	spackLoadUnique trilinos~cuda
fi

if ! command -v cmake >/dev/null 2>&1 ; then
  spackLoadUniqueNoX cmake@3.17.1:
fi
