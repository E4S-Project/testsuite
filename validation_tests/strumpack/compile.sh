#!/bin/bash -ex
. ./setup.sh
#retVal=$?
#if [ $retVal -ne 0 ] ; then
#  exit $retVal
#fi
#spackLoadUnique strumpack
#spackLoadUnique openblas threads=openmp
#spack load openblas threads=openmp
#spack load metis
#spack load parmetis
#spack load netlib-scalapack
#spack load strumpack
#spack load mpich

#if command -v CC &> /dev/null
#then
#	export CXX=CC
#else
#	export CXX=mpicxx
#fi
#echo "CXX is ${CXX}"


#eval 
${TEST_CXX_MPI} -fopenmp ./test_sparse_mpi.cpp -I${METIS_ROOT}/include -I${BUTTERFLYPACK_ROOT}/include -I${STRUMPACK_ROOT}/include -L${METIS_ROOT}/lib -L${PARMETIS_ROOT}/lib -L${NETLIB_SCALAPACK_ROOT}/lib -L${OPENBLAS_ROOT}/lib -L${STRUMPACK_ROOT}/lib  -lmetis -lopenblas -lstrumpack -lscalapack -lzbutterflypack -ldbutterflypack #-lgfortran -lmpifort
