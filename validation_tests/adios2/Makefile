CC = mpicc
CXX = mpic++
CFLAGS = -I$ADIOS2_ROOT/include -Wall
CXXFLAGS = -I$ADIOS2_ROOT/include -Wall
LD = mpic++
LDFLAGS = -L$ADIOS2_ROOT/lib/ -ladios2_cxx11

all: hello-world

hello-world: hello-world.o
	$(LD) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o hello-world

.PHONY: clean

