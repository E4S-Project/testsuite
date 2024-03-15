#!/bin/bash

source ../../setup.sh

THISDIR=`basename "$PWD"`

if [ "$THISDIR" = "vtk-m-cuda" ];then
  spackLoadUnique "vtk-m+cuda"
else
	spackLoadUnique "vtk-m"
fi
spackLoadUniqueNoX cmake
