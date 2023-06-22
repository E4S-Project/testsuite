NAME=cuda

PAPI_CUDA_ROOT ?= $(shell dirname $(shell dirname $(shell which nvcc)))
PAPIROOT ?= $(PAPI_ROOT)

TESTS = HelloWorld

NVCC = $(PAPI_CUDA_ROOT)/bin/nvcc
NVCC_VERSION := $(shell nvcc --version | grep -oP '(?<=release )\d+\.\d+')
ifeq ($(shell echo "$(NVCC_VERSION) >= 11.6" | bc), 1)
    NVCFLAGS := -arch=native
else
    NVCFLAGS :=
endif

PAPI_FLAG = -DPAPI    # Comment this line for tests to run without PAPI profiling
NVCFLAGS += -g -ccbin='$(CC)' $(PAPI_FLAG)
INCLUDE += -I$(PAPIROOT)/include -I$(PAPI_CUDA_ROOT)/include
CUDALIBS = -L$(PAPI_CUDA_ROOT)/lib64 -lcudart -lcuda
PAPILIB += -L$(PAPIROOT)/lib -lpfm -lpapi

cuda_tests: $(TESTS)

%.o:%.cu
	$(NVCC) $(INCLUDE) $(NVCFLAGS) -c -o $@ $<

%.mac:%.cu
	$(NVCC) $(INCLUDE) $(NVCFLAGS) -E -c -o $@ $<

HelloWorld: HelloWorld.o $(UTILOBJS)
	$(NVCC) $(NVCFLAGS) -o HelloWorld HelloWorld.o $(UTILOBJS) $(PAPILIB) $(CUDALIBS) $(LDFLAGS)

clean:
	rm -f *.o $(TESTS) $(TESTS_NOCTX)
