CC = $(TEST_CC)  #$(TEST_CC_MPI) 
CXX = $(TEST_CXX) #$(TEST_CXX_MPI) 
CXXFLAGS := \
        -O0 -g -Wall \
        -march=native -mtune=native \
        -I$(LIBUNWIND_ROOT)/include

LDFLAGS = -L$(LIBUNWIND_ROOT)/lib -lunwind 

all: backtrace

backtrace: backtrace.o
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<


clean:
	rm -f *.o backtrace

.PHONY: clean

