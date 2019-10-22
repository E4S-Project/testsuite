#!/bin/bash
oneSpackHash(){ echo "/`spack find -l $1 | tail -n1 | awk '{print $1;}'`" ;  }
ONESUN=`oneSpackHash sundials`

spack load mpich
spack load $ONESUN

