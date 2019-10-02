#!/bin/bash
spack load openblas threads=none
spack load superlu 
./c_sample
