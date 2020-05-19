#!/bin/bash

MYCC=clang
MYFC=flang

MYOPT="-g -Wall "
MYOPTF="-fno-underscoring -g -Wall "

$MYCC $MYOPT -shared -o libaddc.so libaddc.c
$MYFC $MYOPTF -shared -o libaddf.so libaddf.f90
$MYFC $MYOPT -shared -o libaddf2.so libaddf.f90

$MYCC $MYOPT -o callfunctions_c callfunctions.c -L. -laddc -laddf
$MYCC $MYOPT -o callfunctions2_c callfunctions2.c -L. -laddc -laddf2
$MYFC $MYOPTF -o callfunctions_f callfunctions.f90 -L. -laddc -laddf
