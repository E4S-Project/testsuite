CC = $(TEST_CC_MPI) #mpicc
CFLAGS = -Wall -g
LD = $(TEST_CC_MPI) #mpicc
LDFLAGS = 

all: initfinalize sendrecv broadcast onesided

initfinalize: initfinalize.o
	$(LD) -o $@ $^ $(LDFLAGS)

sendrecv: sendrecv.o
	$(LD) -o $@ $^ $(LDFLAGS)

broadcast: broadcast.o
	$(LD) -o $@ $^ $(LDFLAGS)

onesided: onesided.o
	$(LD) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o initfinalize sendrecv broadcast onesided

.PHONY: clean
