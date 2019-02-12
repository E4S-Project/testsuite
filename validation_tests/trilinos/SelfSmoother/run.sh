spack unload openmpi
spack load mpich trilinos hypre parmetis hdf5 metis openblas superlu zlib netcdf matio boost@1.66.0 scalapack suite-sparse

mpirun -np 4 ./SelfSmoother
