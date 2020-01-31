#!/bin/bash -ex
. ./setup.sh
#spack load umpire 

g++ malloc.cxx -I$UMPIRE_ROOT/include/ -L$UMPIRE_ROOT/lib  -lumpire 


