CC = $(TEST_CC)  
CFLAGS = -I$(GLOBALARRAYS_ROOT)/include -Wall -DHAVE_ASSERT_H -DHAVE_STDIO_H
LD = $(TEST_CC)
LDFLAGS = -L$(GLOBALARRAYS_LIB_PATH) -larmci

all: transp1D-c 

transp1D-c: transp1D-c.o
	$(LD) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o transp1D-c

.PHONY: clean

