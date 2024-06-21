CC = $(TEST_CC)  #$(TEST_CC_MPI) 
CXX = $(TEST_CXX) #$(TEST_CXX_MPI) 
CFLAGS = -I$(LOKI_ROOT)/include -Wall
CXXFLAGS = -I$(LOKI_ROOT)/include -Wall 
LD = $(TEST_CXX) #$(TEST_CXX_MPI) #mpic++
LDFLAGS = -L$(LOKI_LIB_PATH) -lloki

BIN := checker
SRC := checker.cpp
OBJ := $(SRC:.cpp=.o)

.PHONY: all clean
all: $(BIN)
clean:
	rm -f $(BIN)
	rm -f $(OBJ)

$(BIN): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
