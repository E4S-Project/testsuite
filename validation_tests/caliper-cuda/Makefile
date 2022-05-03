CXX = nvcc
LDFLAGS = -L$(CALIPER_LIB_PATH) -lcaliper -L$(CUDA_ROOT)/extras/CUPTI/lib64 -lcupti -L$(CUDA_LIB_PATH)

caliper: caliperTest.o
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cu
	$(CXX) -I$(CALIPER_ROOT)/include -c -I$(CUDA_ROOT)/extras/CUPTI/include -I$(CUDA_ROOT)/include $< 

clean: 
	rm -f *.o *.log tmp.* caliper

.PHONY: clean
	
