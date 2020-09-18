CXX = g++
LD = g++

CXXFLAGS = `pkg-config --cflags faodel`
LDFLAGS = `pkg-config --libs faodel`

all: singleton_example

singleton_example: singleton_example.o libServiceA.so libServiceB.so libServiceC.so
	$(LD) -o $@ $^ $(LDFLAGS) -L. -lServiceA -lServiceB -lServiceC

libServiceA.so: ServiceA.o
	$(LD) -shared -fpic -o $@ $^ $(LDFLAGS)

libServiceB.so: ServiceB.o
	$(LD) -shared -fpic -o $@ $^ $(LDFLAGS)

libServiceC.so: ServiceC.o
	$(LD) -shared -fpic -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -fpic -c $<

clean:
	rm -f *.o singleton_example *.so

.PHONY: clean
