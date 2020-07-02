## Required environment variables

* `TAU`: path to the TAU installation (with architecture). Example: `$HOME/tau2/x86_64`
* `TAU_MAKEFILE`: name of the TAU makefile. Example: `shared-clang-ompt-v5-pdt-openmp`
* `TAU_FUNCTIONS`: path to the file containing the name of the function we are profiling. See `functions_C.txt` in the durrect directory.
* `LLVM_DIR`: path to the LLVM installation

## Benchmarks

"Working" means that the code runs. Some more optimization might be useful.
* Working: gemm  jacobi-1d-imper  jacobi-2d-imper 2mm adi floyd-warshall seidel-2d.py
* In progress: 

## TODO

* More benchmarks
* Refactor findMin.py
