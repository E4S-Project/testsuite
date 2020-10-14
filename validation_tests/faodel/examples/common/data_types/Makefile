CXX = $(TEST_CXX) #g++
LD = $(TEST_CXX) #g++

CXXFLAGS = `pkg-config --cflags faodel`
LDFLAGS = `pkg-config --libs faodel`

all: data_types

data_types: nodeid_example.o resourceurl_example.o main.o
	$(LD) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o data_types

.PHONY: clean
