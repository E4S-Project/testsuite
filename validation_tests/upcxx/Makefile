# This Makefile demonstrates the recommended way to build simple UPC++ programs.
# Note this uses some GNU make extensions for conciseness.
#
# To use this makefile, set the UPCXX_INSTALL variable to the upcxx install directory, e.g.
# make UPCXX_INSTALL=<myinstalldir> hello-world
# or (for bash)
# export UPCXX_INSTALL=<myinstalldir>; make hello-world

ifeq ($(UPCXX_INSTALL),)
$(warning UPCXX_INSTALL environment variable is not set, assuming upcxx is in the PATH)
UPCXX=upcxx
else
ifeq ($(wildcard $(UPCXX_INSTALL)/bin/upcxx),)
$(error Please set UPCXX_INSTALL=/path/to/upcxx/install)
else
UPCXX=$(UPCXX_INSTALL)/bin/upcxx
endif
endif

UPCXX_THREADMODE ?= seq
export UPCXX_THREADMODE
UPCXX_CODEMODE ?= debug
export UPCXX_CODEMODE
CXX = $(UPCXX)

CXXFLAGS = # optional command-line override

# Programs to build, assuming each has a corresponding *.cpp file
EXAMPLES = \
  dmap-test \
  dmap-async-insert-test \
  dmap-async-find-test \
  dmap-quiescence-test \
  dmap-atomics-test \
  dmap-conjoined-test \
  dmap-promises-test \
  rb1d \
  hello-world \
  view-histogram1 \
  view-histogram2 \
  view-matrix-tasks \
  non-contig-example \
  team_simple \
  team_advanced \
  persona-example-rputs \
  persona-example	\
  serial-custom \
  serial-fields \
  serial-recursive \
  serial-values 


CUDA_EXAMPLES = \
  h-d \
  h-d-remote

all: $(EXAMPLES)
cuda: $(CUDA_EXAMPLES)

# The rule for building any example.
%: %.cpp $(wildcard *.h) $(wildcard *.hpp)
	$(CXX) $@.cpp $(CXXFLAGS) -o $@


# Example-specific variable specializations.
PAR_EXAMPLES = \
  persona-example \
  persona-example-rputs \
  view-matrix-tasks
$(PAR_EXAMPLES): UPCXX_THREADMODE=par

clean:
	rm -f $(EXAMPLES)
	rm -f $(CUDA_EXAMPLES)

.PHONY: clean all

