CC = $(TEST_CC)  #$(TEST_CC_MPI) 
CXX = #$(TEST_CXX_MPI) 
CFLAGS = -I$(ALQUIMIA_ROOT)/include -Wall -DALQUIMIA_HAVE_PFLOTRAN
LD = $(TEST_CC) #mpic++
LDFLAGS = -L$(ALQUIMIA_LIB_PATH) -lalquimia -lpflotran_petsc_stubs

all: test_alquimia_c_utils 

test_alquimia_c_utils: test_alquimia_c_utils.o
	$(LD) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<


clean:
	rm -f *.o test_alquimia_c_utils 

.PHONY: clean

