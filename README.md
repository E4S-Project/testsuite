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

3. Source setup.sh

```
source setup.sh
```


To run all the test run 

```
./test-all.sh
```

Alternately you can run a subset of tests by specifying a directory path in `validation_tests` for instance if you want to run the qthreads test run

```
./test-all.sh ./validation_tests/qthreads
```

The output builds and logs will be in the package directory under validation_tests.
