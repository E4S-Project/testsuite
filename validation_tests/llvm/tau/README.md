## TAU plugin 

Some tests for the TAU plugin, in C and C++.

Each executable has a separate directory, with different potential function listings to test different instrumentations.

Set the following environment variables:
```
LLVM for the install of LLVM to use (default system accessible)
PLUGIN_DIR if the plugin is not installed with LLVM (default in the above LLVM)
TAU_INSTALL for the TAU install to use (no default)
```

The test works by calling ./compile.sh, then ./run.sh. To obtain more specific outputs of the test, you can call the following script.

To check instrumentation solely with the function listing files after an execution, you can use: 

./verify.sh functionlistingfile

Warning: verify will work only if a profile file is present at execution. 


For commented code, read ./testfunctions.sh, which contains all the functions of the testing scripts.

The JIT-compiled functions are not profiled.
