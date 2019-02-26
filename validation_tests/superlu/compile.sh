#!/bin/bash

gcc -g ./c_sample.c  -I/usr/local/packages/ecp/spack/opt/spack/linux-centos7-x86_64/gcc-7.3.0/superlu-5.2.1-igoqeqrgxzvb4ftdxrab4fr4lfpt7eik//include/ -L/usr/local/packages/ecp/spack/opt/spack/linux-centos7-x86_64/gcc-7.3.0/superlu-5.2.1-igoqeqrgxzvb4ftdxrab4fr4lfpt7eik/lib64 -L/usr/local/packages/ecp/spack/opt/spack/linux-centos7-x86_64/gcc-7.3.0/openblas-0.2.20-tq4e7crjp5kedl55ubqkq5mf77kbdwbp/lib  -lsuperlu -lopenblas -lm -o c_sample




