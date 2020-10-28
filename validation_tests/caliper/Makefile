CC = g++
LDFLAGS = -L$(CALIPER_ROOT)lib64 -lcaliper


caliper: caliperTest.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CC) -c $< 

clean: 
	rm -f *.o *.log caliper

.PHONY: clean
	
