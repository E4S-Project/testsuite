. ../../setup.sh
ONEMFEM=`spackLoadUnique mfem`
spackLoadUnique mfem
#spack load $ONEMFEM 
#spack load /hxeu37v #HDF5
#spack load /6xsnuqx #Trilinos
#spack load /373gdxv #Petsc
#spack load mpich
#spack load parmetis
#spack load `oneSpackHash superlu-dist`
export MFEM_INSTALL_DIR=`spack location --install-dir $ONEMFEM`
