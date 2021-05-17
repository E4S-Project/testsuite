CXX = $(TEST_CXX_MPI) #mpic++ 
LDFLAGS = -L$(CALIPER_LIB_PATH) -lcaliper


caliper: caliperTest.o
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) -I$(CALIPER_ROOT)/include -c $< 

clean: 
	rm -f *.o *.log tmp.* caliper

.PHONY: clean
	
