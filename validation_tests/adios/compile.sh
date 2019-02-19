#!/bin/bash

spack load adios
spack load lz4
spack load zfp
spack load sz
spack load zlib
spack load mpich


#I have not found a way to build the examples without blosc
git clone https://github.com/Blosc/c-blosc.git
mkdir c-blosc-install
export BLOSCDEST=`pwd`/c-blosc-install
cd c-blosc
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=${BLOSCDEST}
make install
cd ../..

wget https://users.nccs.gov/~pnorbert/adios-1.13.1.tar.gz
tar -zxf adios-1.13.1.tar.gz
cd adios-1.13.1

#The examples look in the local src directory for the adios libraries
 ln -s `spack location -i adios`/lib/*.a ./src/


./configure --with-zlib=`spack location -i zlib@1.2.11%gcc@7.3.0` --with-sz=`spack location -i sz` --with-zfp=`spack location -i zfp` --with-lz4=`spack location -i lz4` --with-blosc=${BLOSCDEST}

#The FORTRAN tests fail to build because adios_write_mod.mod is not found
cd examples/C

make
cd ../../../
