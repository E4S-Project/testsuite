HIP_PATH?= $(wildcard /opt/rocm)
HIPCC=$(HIP_PATH)/bin/hipcc

SOURCES = HelloWorld.cpp
OBJECTS = $(SOURCES:.cpp=.o)

EXECUTABLE=./HelloWorld

.PHONY: test


all: $(EXECUTABLE) test

CXXFLAGS =-g  -I$(CALIPER_ROOT)/include  
LDFLAGS = -L$(CALIPER_LIB_PATH) -lcaliper  
CXX=$(HIPCC)


$(EXECUTABLE): $(OBJECTS)
	$(HIPCC)    $(OBJECTS) -o $@  $(LDFLAGS)


test: $(EXECUTABLE)
	$(EXECUTABLE)


clean:
	rm -f $(EXECUTABLE)
	rm -f $(OBJECTS)
	rm -f $(HIP_PATH)/src/*.o
