CXX = $(TEST_CXX_MPI) #mpic++ 
LDFLAGS = -L$(CALIPER_ROOT)lib64 -lcaliper


caliper: caliperTest.o
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) -c $< 

clean: 
	rm -f *.o *.log tmp.* caliper

.PHONY: clean
	
