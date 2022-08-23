CC = $(TEST_CC)  #$(TEST_CC_MPI) 
CFLAGS = -I$(GMP_ROOT)/include -Wall
LD = $(TEST_CC) #mpic++
LDFLAGS = -L$(GMP_LIB_PATH) -lgmp

all: mpz_fact 

mpz_fact: mpz_fact.o
	$(LD) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<


clean:
	rm -f *.o mpz_fact

.PHONY: clean

