CC = $(TEST_CC_MPI) #mpicc
CXX = $(TEST_CXX_MPI) #mpic++
CFLAGS = -I$(ADIOS2_ROOT)/include -Wall
CXXFLAGS =  $(shell adios2-config --cxx-flags -m)   
LD = $(TEST_CXX_MPI) #mpic++
LDFLAGS = $(shell adios2-config --cxx-libs -m)   

all: helloBPTimeWriter 

helloBPTimeWriter: helloBPTimeWriter.o
	$(LD) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<


clean:
	rm -f *.o helloBPTimeWriter 

.PHONY: clean

