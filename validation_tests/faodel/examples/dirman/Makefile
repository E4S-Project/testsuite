CXX = $(TEST_CXX) #mpic++
LD = $(TEST_CXX) #mpic++

CXXFLAGS = `pkg-config --cflags faodel`
LDFLAGS = `pkg-config --libs faodel`

all: dirman1-preload-configuration dirman2-runtime-define

dirman1-preload-configuration: dirman1-preload-configuration.o
	$(LD) -o $@ $^ $(LDFLAGS)

dirman2-runtime-define: dirman2-runtime-define.o
	$(LD) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o dirman1-preload-configuration dirman2-runtime-define

.PHONY: clean
