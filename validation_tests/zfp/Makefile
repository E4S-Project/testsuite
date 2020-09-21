BINDIR = ./
#bin
TARGETS = $(BINDIR)/diffusion\
	  $(BINDIR)/inplace\
	  $(BINDIR)/iterator\
	  $(BINDIR)/pgm\
	  $(BINDIR)/ppm\
	  $(BINDIR)/simple\
	  $(BINDIR)/speed
LIBS = -L$(ZFP_ROOT)/lib -lzfp
INCLUDE=-I$(ZFP_ROOT)/include
CLIBS = $(LIBS) -lm
CXXLIBS = $(LIBS)

all: $(TARGETS)

$(BINDIR)/diffusion: diffusion.cpp   
	$(CXX) $(CXXFLAGS) $(INCLUDE) diffusion.cpp $(CXXLIBS) -o $@

$(BINDIR)/inplace: inplace.c 
	$(CC) $(CFLAGS) inplace.c $(CLIBS) -o $@

$(BINDIR)/iterator: iterator.cpp 
	$(CXX) $(CXXFLAGS) $(INCLUDE) iterator.cpp $(CXXLIBS) -o $@

$(BINDIR)/pgm: pgm.c ../lib/$(LIBZFP)
	$(CC) $(CFLAGS) pgm.c $(CLIBS) -o $@

$(BINDIR)/ppm: ppm.c ../lib/$(LIBZFP)
	$(CC) $(CFLAGS) $(PPM_FLAGS) ppm.c $(CLIBS) -o $@

$(BINDIR)/simple: simple.c 
	$(CC) $(CFLAGS) simple.c $(CLIBS) -o $@

$(BINDIR)/speed: speed.c ../lib/$(LIBZFP)
	$(CC) $(CFLAGS) speed.c $(CLIBS) -o $@

clean:
	rm -f $(TARGETS) test.out
