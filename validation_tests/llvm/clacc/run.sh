#!/bin/bash

. ./setup.sh
#./householder 1024 1024

./parallel
./jacobi
./jacobi_data

NB=4
STARTED=`./gang $NB | grep "toto" | wc -l`
if [ $NB == $STARTED ]; then
    echo "Gang: [PASS]"
else
    echo "Gang:[FAIL] launched", $STARTED, " instead of ", $NB, "requested"
fi


./inout
if [ $? == 0 ] ; then
    echo "Data in/out: [PASS]"
else
    echo "Data in/out: [FAIL]"
fi

./inout_data
if [ $? == 0 ] ; then
    echo "Data in/out (2): [PASS]"
else
    echo "Data in/out (2): [FAIL]"
fi

# Offloading stuff

for SOURCE in "jacobi" "data" ; do
    if [ `arch` == "ppc64le" ]; then
	ARCHITECTURES=(nvptx64-nvidia-cuda powerpc64le-unknown-linux-gnu)
    fi
    if [ `arch` == "x86_64" ]; then
	ARCHITECTURES=(nvptx64-nvidia-cuda x86_64-unknown-linux-gnu)
    fi
    
    for TARGET in ${ARCHITECTURES[@]} ; do
	./${SOURCE}_$TARGET
	RET=$?
	echo -n "Running " $SOURCE " with offloading on " $TARGET
	if [ $RET == 0 ] ; then
	    echo " [PASS]"
	else
	    echo " [FAIL]"
	fi
    done
done

