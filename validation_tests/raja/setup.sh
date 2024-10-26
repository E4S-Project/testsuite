#!/bin/bash
. ../../setup.sh

spackLoadUniqueNoX cmake

THISDIR=`basename "$PWD"`
USECUDA=0
USEROCM=0
if [  "$THISDIR" = "raja-cuda" ];then
	spackLoadUnique raja+cuda
	USECUDA=1
elif [  "$THISDIR" = "raja-rocm" ];then
	spackLoadUnique raja+rocm
	USEROCM=1
else
	spackLoadUnique raja~cuda~rocm
fi
