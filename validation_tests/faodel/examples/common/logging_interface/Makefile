CXX = $(TEST_CXX) #g++
LD = $(TEST_CXX) #g++

CXXFLAGS = `pkg-config --cflags faodel`
LDFLAGS = `pkg-config --libs faodel`

all: logging_interface

SRC =  ClassA.cpp   ClassB.cpp   example1_selectiveA.cpp  example2_disableB.cpp  logging_interface.cpp
OBJ = $(SRC:.cpp=.o)

logging_interface: $(OBJ)
	$(LD) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o logging_interface

.PHONY: clean
