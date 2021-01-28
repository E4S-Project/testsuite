## TAU plugin 

Some tests for the TAU plugin, in C and C++.

Each executable has a separate directory, with different potential function listings to test different instrumentations.

Set the following environment variables:
```
LLVM for the install of LLVM to use (default system accessible)
PLUGIN_DIR if the plugin is not installed with LLVM (default in the above LLVM)
TAU_INSTALL for the TAU install to use (no default)
```

The test works by calling ./compile.sh, then ./run.sh. To obtain more specific outputs of the test, you can call the following scripts.

To  compile, run and check correct instrumentation with a specific function listing file, as well as providing more detailed outputs, you can use:

./cev.sh functionlistingfile


To check instrumentation solely with the function listing files after an execution, you can use: 

./verify.sh functionlistingfile


Warning: This will work only if a profile file is present at execution. Also, this requires an function listing files that doesn't rely on wildcards. 



For commented code, read cev.sh, as it runs the whole TAU plugin workflow.

The JIT-compiled functions are not profiled.
