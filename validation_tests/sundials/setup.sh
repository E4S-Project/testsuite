#!/bin/bash
. ../../setup.sh
#oneSpackHash(){ echo "/`spack find -l $1 | tail -n1 | awk '{print $1;}'`" ;  }
#ONESUN=`oneSpackHash sundials`
TESTNAME=sundials
TESTVERSION=
export THISDIR=`basename "$PWD"`
if [  "$THISDIR" = "$TESTNAME-cuda" ];then
        spackLoadUnique "$TESTNAME+cuda $TEST_CUDA_ARCH $TESTVERSION"
elif [  "$THISDIR" = "$TESTNAME-rocm" ];then
        spackLoadUnique "$TESTNAME+rocm $TEST_ROCM_ARCH $TESTVERSION"
else
        spackLoadUnique $TESTNAME ~cuda~rocm $TESTVERSION
fi


spackLoadUniqueNoX cmake@3.22.1:
#spack load mpich
#spack load $ONESUN

