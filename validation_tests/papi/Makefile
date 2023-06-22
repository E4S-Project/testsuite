EXECUTABLES = simple threads

CC          = gcc -O0 -pthread

PAPIROOT ?= $(PAPI_ROOT)
PAPIINCLUDE	= $(PAPIROOT)/include
PAPILIB		= $(PAPIROOT)/lib
INCLUDE = -I. -I$(PAPIINCLUDE) 

CFLAGS      = -I./ $(INCLUDE)

LIBS := -L$(PAPILIB)  -lm -ldl -lpapi

all: $(EXECUTABLES)

clean:
	/bin/rm -f core *.o $(EXECUTABLES)

i.SUFFIXES: .c .o
	.c.o:
	    $(CC) $(CFLAGS) $(INCLUDE) -c $*.c

simple: simple.o 
	$(CC) $(CFLAGS) $(INCLUDE) -o simple simple.o $(LIBS)
threads: threads.o 
	$(CC) $(CFLAGS) $(INCLUDE) -o threads threads.o $(LIBS)
