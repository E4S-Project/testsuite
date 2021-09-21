CC ?= gcc
CC := $(CC) -rdynamic 

loop: loop.c
	$(CC) -o $@ $<

run:
	hpcrun -e cycles ./loop 10000000000
	hpcstruct loop
	hpcprof -S loop.hpcstruct hpctoolkit-loop-measurements

clean:
	rm -f loop
	rm -r -f hpctoolkit-loop-measurements
	rm -f loop.hpcstruct
	rm -r -f hpctoolkit-loop-database


