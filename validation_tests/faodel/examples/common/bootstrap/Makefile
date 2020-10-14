CXX = $(TEST_CXX) #g++
LD = $(TEST_CXX) #g++

CXXFLAGS = `pkg-config --cflags faodel`
LDFLAGS = `pkg-config --libs faodel`

all: bootstrap_example

bootstrap_example: bootstrap_example.o
	$(LD) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o bootstrap_example

.PHONY: clean
