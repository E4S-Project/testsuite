CC = $(TEST_CC)  #$(TEST_CC_MPI) 
CXX = $(TEST_CXX) #$(TEST_CXX_MPI) 
CFLAGS = -I$(ADIOS2_ROOT)/include -Wall
CXXFLAGS = -I$(ADIOS2_ROOT)/include -Wall
LD = $(TEST_CXX) #$(TEST_CXX_MPI) #mpic++
LDFLAGS = -L$(ADIOS2_LIB_PATH) -ladios2_cxx11

all: hello-world

hello-world: hello-world.o
	$(LD) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<


clean:
	rm -f *.o hello-world

.PHONY: clean

