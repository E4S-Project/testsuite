CC ?= gcc
CC := $(CC) -rdynamic 

loop: loop.c
	$(CC) -o $@ $<

run:
	hpcrun -o hpctoolkit-loop-measurements -e REALTIME ./loop 10000000000
	hpcstruct --nocache hpctoolkit-loop-measurements
	hpcprof -o hpctoolkit-loop-database hpctoolkit-loop-measurements

clean:
	rm -f loop
	rm -r -f hpctoolkit-loop-measurements
	rm -f loop.hpcstruct
	rm -r -f hpctoolkit-loop-database


