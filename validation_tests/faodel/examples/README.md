FAODEL Examples
===============

The FAODEL project provides a set of different libraries that
developers can use to create complex data management services. Each
library provides its own set of examples and tutorials to help
developers get started with the software. The examples are located in
the examples directory of each project.

Examples Philosophy
-------------------
We assume that most developers just want to start using the
FAODEL libraries without having to dig into the guts of the
source code or its (admittedly) complex build system. Thus, our
philosophy for providing examples is that they should be stand-alone
code that's outside of the core build system, that users can pick up
and use as templates for their own projects. The overall build process
for users is to:

- Download the FAODEL source code
- Build and install the libraries
- Build the examples, using the Faodel installation
  
The general build process is as follows:

```
    # Create build directories for both faodel and examples
    cd faodel
    mkdir {build,build_examples}
    export Faodel_DIR=$(pwd)/build/install
    
    # Build and install faodel
    cd build
    cmake -DCMAKE_INSTALL_PREFIX=$Faodel_DIR ..
    make install
    cd ..
    
    # Build all the examples (using $Faodel_DIR for the library)
    cd build_examples
    cmake -DCMAKE_INSTALL_PREFIX=$(pwd)/install ../examples
    make install

```
