BINDIR=.
TARGETS=$(BINDIR)/umaptest
LIBS=-L$(UMAP_LIB_PATH) -lumap
INCLUDE=-I$(UMAP_ROOT)/include
CXXLIBS=$(LIBS) -std=gnu++11

all: $(TARGETS)

$(BINDIR)/umaptest: umaptest.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) umaptest.cpp $(CXXLIBS) -o $@

clean:
	rm -f $(TARGETS)
