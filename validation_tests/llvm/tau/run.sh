#!/bin/bash

for ex in mm_c mm_cpp householder householder_jit ; do
    tau_exec  -T serial,clang ./${ex}
    pprof
done
