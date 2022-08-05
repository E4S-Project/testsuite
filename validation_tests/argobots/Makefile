CC = $(TEST_CC)  #$(TEST_CC_MPI) 
CXX = #$(TEST_CXX_MPI) 
CFLAGS = -I$(ARGOBOTS_ROOT)/include -Wall
CXXFLAGS = -I$(ARGOBOTS_ROOT)/include -Wall
LD = $(TEST_CC) #mpic++
LDFLAGS = -L$(ARGOBOTS_LIB_PATH) -labt

all: fibonacci

fibonacci: fibonacci.o
	$(LD) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<


clean:
	rm -f *.o fibonacci

.PHONY: clean

