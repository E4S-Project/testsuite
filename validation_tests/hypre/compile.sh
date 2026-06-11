#!/bin/bash
. ./setup.sh
set -x
set -e

${TEST_CC_MPI} -o hypre_smoke_test hypre_smoke_test.c \
    -I${HYPRE_ROOT}/include \
    -L${HYPRE_LIB_PATH} -Wl,-rpath,${HYPRE_LIB_PATH} -lHYPRE -lm \
    ${ARCHARGS}

#make  
