## Testing Fortran Compilers using GFortran's `gfortran.dg` Testsuite

This repo contains a copy of the `trunk/gcc/testsuite/gfortran.dg` directory tree
from the gcc SVN repository, and a `CMakeLists.txt` file from Janus Weil (@janusw)
that filters the tests into a set that are meaningful for a general Fortran compiler.

From the instructions at the top of the `CMakeLists.txt` file:
```
mkdir test_my_compiler; cd test_my_compiler
cmake .. -DFC=my_fortran_compiler
ctest -j$(nproc) -R CMP
ctest -j$(nproc) -R EXE
```
It sets particular compiler flags if "my_fortran_compiler" is `gfortran`, `ifort`, or `nagfor`.
Other compilers may or may not require specific flags (for things like coarray support).

My particular interest here is the NAG compiler, and my intent for the repo is merely to
track and share issues with the test suite exposed by nagfor (see the Issues) as well as
as the tests that nagfor fails.  See the Wiki page for the current NAG results and detailed
breakdown of the tests.  I expect this repo will have a relatively short lifespan.

Feel free to clone the repo and test your favorite Fortran compiler as Janus had
[urged](https://groups.google.com/forum/#!topic/comp.lang.fortran/AIHRQ2kJv3c).
