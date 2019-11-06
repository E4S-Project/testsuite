#!/bin/bash
. ../../setup.sh
spackLoadUnique adios

#oneSpackHash(){ echo "/`spack find -l $1 | tail -n1 | awk '{print $1;}'`" ;  }
#spack load lz4
#spack load snappy
#spack load zstd
#spack load `oneSpackHash zfp`
#spack load `oneSpackHash sz`
#spack load `oneSpackHash zlib`
#spack load c-blosc
#spack load mpich
#spack load adios
