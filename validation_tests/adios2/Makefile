CC = $(TEST_CC) #mpicc
CXX = $(TEST_CXX) #mpic++
CFLAGS = -I$ADIOS2_ROOT/include -Wall
CXXFLAGS = -I$ADIOS2_ROOT/include -Wall
LD = $(TEST_CXX) #mpic++
LDFLAGS = -L$ADIOS2_ROOT/lib/ -ladios2_cxx11

all: hello-world

hello-world: hello-world.o
	$(LD) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o hello-world

.PHONY: clean

