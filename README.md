# testsuite
E4S test suite with validation tests

# Getting Started

1. Clone E4S Testsuite

```
git clone https://github.com/E4S-Project/testsuite
```

2. Source your spack environment

```
source /path/to/spack/share/spack/setup-env.sh
```

3. Check settings.sh. By default the testsuite will get its compile and run commands from settings.sh in the top level testsuite directory. You can smylink this file to one of the settings preset files or indicate a file with an argument to test-all.sh.


To run all the test run 

```
./test-all.sh
```

The output builds and logs will be in the test directories.

The return value of test-all.sh is the number of failures encountered (0 for complete success).

Alternately you can run a subset of tests by specifying a directory path in `validation_tests` for instance if you want to run the qthreads test run

```
./test-all.sh ./validation_tests/qthreads
```

Test subsets may be created by symlinking selected test directories from validation_tests to a new directory.

After the optional test directory argument test-all.sh accepts to additional optional arguments:
```
--json
```
will cause the test output to be written in json format. Redirect to a file for post-processing.


```
--settings <file>
```
allows you to specify a settings file that provides the compile and run commands to be used by the testsuite. By default the file named settings.sh will be used. This can be symlinked to provided sample settings failes.

Variables that can be set in the settings file:

* TEST_CC_MPI: MPI C compiler command
* TEST_CXX_MPI: MPI C++ compiler command
* TEST_FTN_MPI: MPI Fortran compiler command
* TEST_CC: C compiler command
* TEST_CXX: C++ compiler command
* TEST_FTN: Fortran compiler command
* TEST_RUN_CMD: MPI run command
* TEST_RUN_PROCFLAG: flag to indicate number of processes to MPI
* TEST_RUN_PROCARG: number of MPI processes to run
* TEST_RUN: Combined full mpi invocation including number of processes
* TESTSUITE_VARIANT: Variant that will be applied to every spack package search when setting up test environments. Useful when spack packages are built with multiple compilers. Optional.
* Any global environment variable to be applied to every test.

These can be used in combination within test build and run scripts to make sure consistent sane defaults for runs on a given system. Specific tests can override these values if needed (e.g. a test that is hard coded to run ona certain number of MPI processes.

```
--print-logs
```
Print contents of all clean/compiler/run logs to screen as the tests complete

```
--color-off
```
Disable printing test results in color

# Creating Tests

Create a new test by placing a directory in the validation_tests directory. The directory name should be descriptive of the test. The name of the spack package being tested is a reasonable default.

The actual test code and build files should be set up to work with includes and libraries provided by spack. To simplify this process we provide environment variables that can be referenced to get the locations of these resources.

In addition to any build, code and data files the test directory should generally contain four scripts:

## setup.sh
This sets up the environment for the clean/compile/run operations. A basic setup.sh script will look like:

    #!/bin/bash
    . ../../setup.sh  #The top level
    spackLoadUnique <package name>

`spackLoadUnique` is made available by sourcing the top level setup.sh script. This command loads a single version of the spack package specified along with all of its dependencies. It also sets environment variables that are useful for building/running with your package. `<PACKAGE NAME>_ROOT`  will provide the install location of the package loaded by spack. `<PACKAGE_NAME>_LIB_PATH` gives the full path of the library directory in the package installation. This is useful since there can be ambiguity between the use of lib and lib64 directories for certain packages. Multiple spackLoadUnique calls can be run if needed. Other environment variables can also be set here.

## clean.sh
This cleans the test directory. A basic clean.sh script will look like:

    #!/bin/bash
    . ./setup.sh
    make distclean

This assumes the test uses a makefile with a distclean target. Executing `make clean` or manually running `rm` on generated build or output files and directories is also possible. Sourcing the local setup.sh may be optional if the clean operation doesn't depend on having the package loaded.

## compile.sh
The compile scripts builds the test. In the case of a test that builds with a simple call to `make` the script might look like:

    #!/bin/bash -e
    . ./setup.sh
    set -x
    make 
Using -e is recommended to cause the script to exit at the first failure. Setting -x is helpful in getting verbose output from internal scripts. Depending on the nature of your test this script might also sett build specific environment variables, directly invoke compiler commands, create and `cd` to a build directory and invoke cmake.

This script could potentially iterate through multiple build directories to create more than one executable. This script may be optional if no build is necessary, as may be the case for some python packages.

## run.sh
Runs the executables created for the test. A simple run script might look like:

    #!/bin/bash -e
    . ./setup.sh
    set -x
    eval $TEST_RUN ./<executable> #Using the TEST_RUN variable provided by settings.sh to run with MPI
    ./<executable> #Running a non-mpi invocation of an executable

This script could potentially iterate through the execution of multiple executables.
