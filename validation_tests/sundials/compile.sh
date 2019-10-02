#!/bin/bash
. ../../setup.sh
spack load sundials
spack load mpich

#cp -r `spack location -i sundials`/examples .

cd examples/nvector/parallel/

make  
