CC = $(TEST_CC)  #$(TEST_CC_MPI) 
CFLAGS = -I$(GMP_ROOT)/include -Wall
LD = $(TEST_CC) #mpic++
LDFLAGS = -L$(GMP_LIB_PATH) -lgmp

all: t-rand 

mpz_fact: t-rand.o
	$(LD) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<


clean:
	rm -f *.o t-rand

.PHONY: clean

