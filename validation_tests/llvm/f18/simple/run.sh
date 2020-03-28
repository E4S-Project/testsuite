#!/bin/bash

echo -e "\033[01;34m[Special characters handling]\e[0m"

echo "Compiling with gfortran"
make FC=gfortran
make clean

echo "Compiling with f18"
make FC=f18
make clean

echo -e "\033[01;34m[Backends]\e[0m"

runtests (){
    export F18_FC=$1
    echo "Using "$1
    
    if [ "$F18_FC" = "clang" ] ; then LIBS="-lgfortran" ; fi

    for INFILE in hello.f95 helloworld3.f  ; do
	f18 -o hello $INFILE $LIBS
	RET=$?
	echo -n $INFILE " compilation: "
	if [ $RET == 0 ] ; then
	    echo -e "\033[01;32m[PASS]\e[0m"
	    ./hello > /dev/null
	    RET=$?
	    echo -n $INFILE "  execution:  "
	    if [ $RET == 0 ] ; then
		echo -e "\033[01;32m[PASS]\e[0m"
	    else
		echo -e "\033[01;31m[FAIL]\e[0m"
	    fi
	else
	    echo -e "\033[01;31m[FAIL]\e[0m"
	fi
	rm hello
    done
    
    INFILE=summation.f 
    f18 -o hello $INFILE $LIBS
    RET=$?
    echo -n $INFILE " compilation: "
    if [ $RET == 0 ] ; then
	echo -e "\033[01;32m[PASS]\e[0m"
	echo "120" | ./hello > /dev/null
	RET=$?
	echo -n $INFILE "  execution:  "
	if [ $RET == 0 ] ; then
	    echo -e "\033[01;32m[PASS]\e[0m"
	else
	    echo -e "\033[01;31m[FAIL]\e[0m"
	fi
    else
	echo -e "\033[01;31m[FAIL]\e[0m"
    fi
    rm hello
    
}

runtests "gfortran" 
runtests "clang" 



#test_dot.f90; do

