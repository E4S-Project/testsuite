#!/bin/bash

. ./setup.sh

# Compilation flags tests

clang -fopenacc -o parallel parallel.c
RET=$?
echo -n "Simple compilation  " 
if [ $RET == 0 ] ; then
    echo "[PASS]"
else
    echo "[FAIL]"
fi
clang -fno-openacc -o parallel parallel.c
OUTFILE=toto
ERRFILE=titi
for FLAG in "-fopenacc-print=acc" "-fopenacc-print=omp " "-fopenacc-print=acc-omp " "-fopenacc-print=omp-acc" ; do
    clang $FLAG -o parallel parallel.c > $OUTFILE
    RET=$?
    echo -n $FLAG "  "
    if [ $RET == 0 ] ; then
	echo "[PASS]"
    else
	echo "[FAIL]"
    fi
done

clang -Wsource-uses-openacc -o parallel parallel.c > $OUTFILE 2> $ERRFILE
RET=$?
echo -n "-Wsource-uses-openacc  " 
if [ $RET == 0 ] ; then
    if [ `grep warning $ERRFILE | wc -l` -gt 0 ] ; then
	echo "[PASS]"
    else
	echo "[FAIL] compiled but no warning generated"
    fi
else
    echo "[FAIL]"
fi
clang -Wopenacc-ignored-clause -o kernel kernel.c > $OUTFILE 2> $ERRFILE
RET=$?
echo -n "-Wopenacc-ignored-clause  " 
if [ $RET == 0 ] ; then
    if [ `grep warning $ERRFILE | wc -l` -gt 0 ] ; then
	echo "[PASS]"
    else
	echo "[FAIL] compiled but no warning generated"
    fi
else
    echo "[FAIL]"
fi


#-fopenmp-targets=<triples> for traditional compilation mode
# nvptx64-nvidia-cuda
# powerpc64le-unknown-linux-gnu
# x86_64-unknown-linux-gnu

for SOURCE in "jacobi" "data" ; do
    if [ `arch` == "ppc64le" ]; then
	ARCHITECTURES=(nvptx64-nvidia-cuda powerpc64le-unknown-linux-gnu)
    fi
    if [ `arch` == "x86_64" ]; then
	ARCHITECTURES=(nvptx64-nvidia-cuda x86_64-unknown-linux-gnu)
    fi
    
    for TARGET in ${ARCHITECTURES[@]} ; do
	clang -O3 -fopenacc -fopenmp-targets=$TARGET -o ${SOURCE}_$TARGET ${SOURCE}.c
	RET=$?
	echo -n "Compile with target " $TARGET
	if [ $RET == 0 ] ; then
	    echo " [PASS]"
	else
	    echo " [FAIL]"
	fi
    done
done

# Compile things we are going to run

clang -fopenacc -o parallel parallel.c
clang -fopenacc -o gang gang.c

# Small tests

clang -fopenacc -Wall -o inout inout.c 
clang -fopenacc -Wall -o inout_data inout_data.c 

clang -fopenacc -Wall -o jacobi jacobi.c 
clang -fopenacc -Wall -o jacobi_data jacobi_data.c 

make
