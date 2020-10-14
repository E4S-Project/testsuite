CXX = $(TEST_CXX) #g++
LD = $(TEST_CXX) #g++

CXXFLAGS = `pkg-config --cflags faodel`
LDFLAGS = `pkg-config --libs faodel`

all: info_interface

info_interface: info_interface.o
	$(LD) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o info_interface

.PHONY: clean
