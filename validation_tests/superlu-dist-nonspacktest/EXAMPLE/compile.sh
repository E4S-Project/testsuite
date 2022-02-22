#!/bin/bash

#. ../setup.sh
#mpicc 
${TEST_CC_MPI} \
 -I${SUPERLU_DIST_ROOT}/include  \
-DUSE_VENDOR_BLAS -fopenmp -std=c99 -O3 -DPRNTlevel=1 -DDEBUGlevel=0 -fPIE \
-o pddrive.c.o -c pddrive.c

#mpicc 
${TEST_CC_MPI} \
 -I${SUPERLU_DIST_ROOT}/include \
-DUSE_VENDOR_BLAS -fopenmp -std=c99 -O3 -DPRNTlevel=1 -DDEBUGlevel=0 -fPIE \
-o dcreate_matrix.c.o -c dcreate_matrix.c

#mpicxx 
${TEST_CXX_MPI} \
-fopenmp -std=c++11 -rdynamic -O3 \
pddrive.c.o dcreate_matrix.c.o -o pddrive \
${SUPERLU_DIST_ROOT}/lib/libsuperlu_dist.a \
/opt/cray/pe/libsci/21.08.1.2/GNU/9.1/x86_64/lib/libsci_gnu_82_mpi_mp.a \
${PARMETIS_ROOT}/lib/libparmetis.so \
${METIS_ROOT}/lib/libmetis.so \
-lm
