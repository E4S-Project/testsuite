#!/bin/bash
. ./setup.sh
set -e
set -x

# PLASMA does its own OpenMP-based scheduling 
export OMP_NUM_THREADS=4

# Prevent nested-thread explosion: the linked BLAS/LAPACK must run single-threaded,
# since PLASMA already parallelizes at the algorithmic level above BLAS.
export OPENBLAS_NUM_THREADS=1
export MKL_NUM_THREADS=1
export GOTO_NUM_THREADS=1
export VECLIB_MAXIMUM_THREADS=1
export BLIS_NUM_THREADS=1

# Small, representative subset
test_list="dgemm dgetrf dpotrf dgeqrf"

# The whole enchalada
#test_list="dzamax damax scamax samax zcposv dsposv zgbmm dgbmm cgbmm sgbmm zgbsv dgbsv cgbsv sgbsv zgbtrf dgbtrf cgbtrf sgbtrf zgeadd dgeadd cgeadd sgeadd zgeinv dgeinv cgeinv sgeinv zgelqf dgelqf cgelqf sgelqf zgelqs dgelqs cgelqs sgelqs zgels dgels cgels sgels zgemm dgemm cgemm sgemm zgeqrf dgeqrf cgeqrf sgeqrf zgeqrs dgeqrs cgeqrs sgeqrs zcgesv dsgesv zcgbsv dsgbsv zgesdd dgesdd cgesdd sgesdd zgesv dgesv cgesv sgesv zgetrf dgetrf cgetrf sgetrf zgetri dgetri cgetri sgetri zgetri_aux dgetri_aux cgetri_aux sgetri_aux zgetrs dgetrs cgetrs sgetrs zhemm chemm zher2k cher2k zherk cherk zhetrf dsytrf chetrf ssytrf zhesv dsysv chesv ssysv zlacpy dlacpy clacpy slacpy zlag2c clag2z dlag2s slag2d zlange dlange clange slange zlanhe clanhe zlansy dlansy clansy slansy zlantr dlantr clantr slantr zlascl dlascl clascl slascl zlaset dlaset claset slaset zgeswp dgeswp cgeswp sgeswp zlauum dlauum clauum slauum zpbsv dpbsv cpbsv spbsv zpbtrf dpbtrf cpbtrf spbtrf zlangb dlangb clangb slangb zposv dposv cposv sposv zpoinv dpoinv cpoinv spoinv zpotrf dpotrf cpotrf spotrf zpotri dpotri cpotri spotri zpotrs dpotrs cpotrs spotrs dstevx2 zsymm dsymm csymm ssymm zsyr2k dsyr2k csyr2k ssyr2k zsyrk dsyrk csyrk ssyrk ztradd dtradd ctradd stradd ztrmm dtrmm ctrmm strmm ztrsm dtrsm ctrsm strsm ztrtri dtrtri ctrtri strtri zunmlq dormlq cunmlq sormlq zunmqr dormqr cunmqr sormqr"

# Iterate through the list and run each program with plasmatest
for program in $test_list; do
    plasmatest "$program"
done

