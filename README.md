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
allow you to specify a settings file that provides the compile and run commands to be used by the testsuite.

The output builds and logs will be in the test directory.

The return value of test-all.sh is the number of failures encountered (0 for complete success).
