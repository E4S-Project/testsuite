PAPI_ROCM_ROOT ?= $(ROCM_ROOT) #/path/to/rocm/

CC        = $(PAPI_ROCM_ROOT)/bin/hipcc
CXX       = $(PAPI_ROCM_ROOT)/bin/hipcc
CXXFLAGS += -g -O2 -fopenmp
CPPFLAGS += -I$(PAPI_ROCM_ROOT)/include -I$(PAPI_ROOT)/include
LDFLAGS  += -L$(PAPI_ROOT)/lib -lpapi -fopenmp

ALL: single_monitor multi_monitor overflow

single_monitor: single_monitor.o matmul.o

multi_monitor: multi_monitor.o matmul.o

overflow: overflow.o matmul.o

run:
	./run_rocm_tests.sh

clean:
	rm -f *.o single_monitor multi_monitor overflow
