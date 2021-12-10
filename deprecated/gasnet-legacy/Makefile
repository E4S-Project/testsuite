CC=$(TEST_CC) #mpicc
LD=$(TEST_CC) #mpicc
CFLAGS = `pkg-config --cflags gasnet-mpi-par` -I.
LDFLAGS = `pkg-config --libs gasnet-mpi-par`

all: testbarrier testhello  testsmall  testthreads  testqueue

testhello: testhello.o
	$(LD) -o $@ $^ $(LDFLAGS)

testsmall: testsmall.o
	$(LD) -o $@ $^ $(LDFLAGS)

testthreads: testthreads.o
	$(LD) -o $@ $^ $(LDFLAGS)

testqueue: testqueue.o
	$(LD) -o $@ $^ $(LDFLAGS)

testbarrier: testbarrier.o
	$(LD) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o testbarrier testhello  testsmall  testthreads  testqueue

.PHONY: clean
