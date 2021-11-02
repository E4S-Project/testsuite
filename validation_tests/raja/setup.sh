#!/bin/bash
. ../../setup.sh

THISDIR=`basename "$PWD"`
if [  "$THISDIR" = "raja-cuda" ];then
	spackLoadUnique raja+cuda
else
	spackLoadUnique raja~cuda~rocm
fi
