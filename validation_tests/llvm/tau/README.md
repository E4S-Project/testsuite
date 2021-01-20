## TAU plugin 

Some tests for the TAU plugin, in C and C++.

Each executable has a separate directory, with different potential function listings to test different instrumentations.

The test functions by calling ./compile.sh, then ./run.sh. To obtain more specific outputs of the test, you can call the following scripts.

To  compile, run and check correct instrumentation with a specific function listing file, you can use:

./cev.sh functionlistingfile

To check instrumentation solely with the function listing files after an execution, you can use: 

./verify.sh functionlistingfile

Warning: This will work only if a profile file is present at execution. Also, this requires an function listing files that doesn't relie on wildcards. 

The JIT-compiled functions are not profiled.
