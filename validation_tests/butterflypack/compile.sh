#!/bin/bash

set -e
. ../setup.sh
mpif90 \
-DHAVE_TASKLOOP \
-DHAVE_MPI3 \
-DHAVE_ARPACK \
-fopenmp \
-ffree-line-length-none \
-ffixed-line-length-none \
-fimplicit-none \
-lpthread -cpp -O3 -DNDEBUG \
-c EMCURV_Module.f90 -o EMCURV_Module.f90.o

mpif90 \
-DHAVE_TASKLOOP \
-DHAVE_MPI3 \
-DHAVE_ARPACK \
-fopenmp \
-ffree-line-length-none \
-ffixed-line-length-none \
-fimplicit-none \
-lpthread \
-cpp  -O3 -DNDEBUG \
-c EMCURV_Driver.f90 -o EMCURV_Driver.f90.o

mpif90 \
-DHAVE_TASKLOOP \
-DHAVE_MPI3 \
-DHAVE_ARPACK \
-fopenmp \
-ffree-line-length-none \
-ffixed-line-length-none \
-fimplicit-none \
-lpthread \
-cpp  -O3 -DNDEBUG -O3 \
EMCURV_Driver.f90.o EMCURV_Module.f90.o \
-o ie2d \
-Wl,-rpath,${OPENBLAS_ROOT}/lib:${NETLIB_LAPACK_ROOT}/lib64:${NETLIB_SCALAPACK_LIB}/lib:${ARPACK_NG_ROOT}/lib64:${BUTTERFLYPACK_ROOT}/lib \
${BUTTERFLYPACK_ROOT}/lib/libzbutterflypack.so \
${OPENBLAS_ROOT}/lib/libopenblas.so \
${NETLIB_LAPACK_ROOT}/lib64/liblapack.so \
${NETLIB_SCALAPACK_ROOT}/lib/libscalapack.so \
${ARPACK_NG_ROOT}/lib64/libarpack.so \
${ARPACK_NG_ROOT}/lib64/libparpack.so \
-lm
