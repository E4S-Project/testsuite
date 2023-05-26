CC = $(TEST_CC)  #$(TEST_CC_MPI) 
CFLAGS = -I$(GMP_ROOT)/include -Wall
LD = $(TEST_CC) #mpic++
LDFLAGS = -L$(GMP_LIB_PATH) -lgmp

all: t-rand 

t-rand: t-rand.o
	${CC} $<  $(LDFLAGS) -o $@

t-rand.o: t-rand.c
	$(CC)  -c $<   $(CFLAGS) 

clean:
	rm -f *.o t-rand

.PHONY: clean

