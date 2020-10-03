#!/bin/bash

CPROG="barrier-subset.c dist-work.c  quo-time.c rebind.c trivial.c"
FPROG="quofort.f90"

for c in $CPROG; do
    n=`echo $c | sed "s/.c$//"`
    rm -f $n
done

for c in $FPROG; do
    n=`echo $c | sed "s/.f90$//"`
    rm -f $n
done
rm -f *.o

