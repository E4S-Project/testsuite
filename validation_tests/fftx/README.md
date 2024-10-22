Demo FFTX External Application
==============================

This is the public repository for the Demo FFTX External Application

### Building Demo FFTX External Application

This demo application provides examples of how to call FFTX libraries and how to
use the APIs to call and link to the libraries.  To use and build the demo
applications you must have previously installed **FFTX** and **spiral-software**. Ensure
your environment sets **FFTX_HOME** and **SPIRAL_HOME** to point to the
locations of FFTX and Spiral.

NOTE: **spiral-software** is only needed because some of the examples include files
distributed with Spiral.  Applications that don't include such files will not
[necessarily] require a **spiral-software** installation.

### Installing Pre-requisites

Clone **spiral-software** to a location on your computer.  E.g., do:
```
cd ~/work
git clone https://www.github.com/spiral-software/spiral-software
```
This location is known as *SPIRAL HOME* and you must set an environment variable
**SPIRAL_HOME** to point to this location later.

If you plan to generate source code from Spiral scripts (e.g., building FFTX
requires this; however, many application users will **not**), then you must also
install two spiral packages, do the following:
```
cd ~/work/spiral-software/namespaces/packages
git clone https://www.github.com/spiral-software/spiral-package-fftx fftx
git clone https://www.github.com/spiral-software/spiral-package-simt simt
```
**NOTE:** The spiral packages must be installed under the directory
**$SPIRAL_HOME/namespaces/packages** and must be placed in folders with the
prefix *spiral-package* removed. 

Follow the build instructions for **spiral-software** (see the **README**
[**here**](https://github.com/spiral-software/spiral-software/blob/master/README.md) ).

### Installing FFTX

Clone **FFTX** to a location on your computer.  E.g., do:
```
cd ~/work
git clone https://www.github.com/spiral-software/fftx
```
Follow the build instructions for **FFTX** (see the **README**
[**here**](https://github.com/spiral-software/FFTX/blob/master/README.md) ).

### Install and Build the Demo Application

Ensure you have valid settings for **FFTX_HOME** and **SPIRAL_HOME**.  Clone the
demo application.  E.g., do:
```
cd ~/work
git clone https://www.github.com/spiral-software/fftx-demo-extern-app
cd fftx-demo-extern-app
mkdir build
cd build
cmake ..
make install
```
The demo application is installed at ~/work/fftx-demo-extern-app/build/bin

### Linking Applications With FFTX

This application has a few small simple applications that link with the FFTX
libraries.  Please review the API calls used in the examples for information on
how to call the transforms within the libraries.  Also, review the
**CMakeLists.txt** for information on how to get the various include file and
linker library paths and library names associated to a target.
