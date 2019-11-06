#!/bin/bash
. ../../setup.sh
#oneSpackHash(){ echo "/`spack find -l $1 | tail -n1 | awk '{print $1;}'`" ;  }
#ONESUN=`oneSpackHash sundials`
spackLoadUnique sundials

#spack load mpich
#spack load $ONESUN

