#!/bin/bash

#. ../setup.sh
mpicc \
-I${SUPERLU_DIST_ROOT}/include  \
-DUSE_VENDOR_BLAS -fopenmp -std=c99 -O3 -DPRNTlevel=1 -DDEBUGlevel=0 -fPIE \
-o pddrive.c.o -c pddrive.c

mpicc \
-I${SUPERLU_DIST_ROOT}/include \
-DUSE_VENDOR_BLAS -fopenmp -std=c99 -O3 -DPRNTlevel=1 -DDEBUGlevel=0 -fPIE \
-o dcreate_matrix.c.o -c dcreate_matrix.c

mpicxx \
-fopenmp -std=c++11 -rdynamic -O3 \
pddrive.c.o dcreate_matrix.c.o -o pddrive \
${SUPERLU_DIST_ROOT}/lib/libsuperlu_dist.a \
${OPENBLAS_ROOT}/lib/libopenblas.so \
${PARMETIS_ROOT}/lib/libparmetis.so \
${METIS_ROOT}/lib/libmetis.so \
-lm
