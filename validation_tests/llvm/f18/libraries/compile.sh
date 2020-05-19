#!/bin/bash

MYCC=clang
MYFC=flang

MYOPT="-g -Wall "
MYOPTF="-fno-underscoring -g -Wall "

# Compile dynamic libraries

$MYCC $MYOPT -shared -o libaddc.so libaddc.c
$MYFC $MYOPTF -shared -o libaddf.so libaddf.f90
$MYFC $MYOPT -shared -o libaddf2.so libaddf.f90

# Link against the dynamic libraries

$MYCC $MYOPT -o callfunctions_c callfunctions.c -L. -laddc -laddf
$MYFC $MYOPTF -o callfunctions_f callfunctions.f90 -L. -laddc -laddf
$MYCC $MYOPT -o callfunctions2_c callfunctions2.c -L. -laddc -laddf2

# Compile static libraries

$MYCC $MYOPT  -c libaddc.c
$MYFC $MYOPTF -c libaddf.f90
$MYFC $MYOPT  -o libaddf2.o -c libaddf.f90

ar cr libaddc.a libaddc.o
ar cr libaddf.a libaddf.o
ar cr libaddf2.a libaddf2.o

# Use them

$MYCC $MYOPT -o callfunctions_c_static callfunctions.c libaddf.a libaddc.a
$MYFC $MYOPTF -o callfunctions_f_static callfunctions.f90 libaddf.a libaddc.a
$MYCC $MYOPT -o callfunctions2_c_static callfunctions2.c libaddf2.a libaddc.a

