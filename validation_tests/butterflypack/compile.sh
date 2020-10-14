#!/bin/bash
. ./setup.sh
#set -e
. ../setup.sh
eval $TEST_FTN \
-DHAVE_TASKLOOP \
-DHAVE_MPI3 \
-DHAVE_ARPACK -I$BUTTERFLYPACK_ROOT/include \
-fopenmp \
-ffree-line-length-none \
-ffixed-line-length-none \
-fimplicit-none \
-lpthread -cpp -O3 -DNDEBUG \
-c EMCURV_Module.f90 -o EMCURV_Module.f90.o

eval $TEST_FTN \
-DHAVE_TASKLOOP \
-DHAVE_MPI3 \
-DHAVE_ARPACK -I$BUTTERFLYPACK_ROOT/include \
-fopenmp \
-ffree-line-length-none \
-ffixed-line-length-none \
-fimplicit-none \
-lpthread \
-cpp  -O3 -DNDEBUG \
-c EMCURV_Driver.f90 -o EMCURV_Driver.f90.o

eval $TEST_FTN \
-DHAVE_TASKLOOP \
-DHAVE_MPI3 \
-DHAVE_ARPACK -I$BUTTERFLYPACK_ROOT/include \
-fopenmp \
-ffree-line-length-none \
-ffixed-line-length-none \
-fimplicit-none \
-lpthread \
-cpp  -O3 -DNDEBUG -O3 \
EMCURV_Driver.f90.o EMCURV_Module.f90.o \
-o ie2d \
-Wl,-rpath,${OPENBLAS_ROOT}/lib:${NETLIB_LAPACK_ROOT}/lib64:${NETLIB_SCALAPACK_LIB}/lib:${ARPACK_NG_ROOT}/lib:${BUTTERFLYPACK_ROOT}/lib \
${BUTTERFLYPACK_ROOT}/lib/libzbutterflypack.so \
${OPENBLAS_ROOT}/lib/libopenblas.so \
${NETLIB_SCALAPACK_ROOT}/lib/libscalapack.so \
${ARPACK_NG_ROOT}/lib/libarpack.so \
${ARPACK_NG_ROOT}/lib/libparpack.so \
-lm
