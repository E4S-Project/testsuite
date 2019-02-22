CC=mpicc
CFLAGS=-g -O3


## ADIOS_DIR/bin should in PATH env
ADIOS_DIR=$(shell adios_config -d)
ADIOS_INC=$(shell adios_config -c )
ADIOS_LIB=$(shell adios_config -l )

default: clean gread_arrays.ch gwrite_arrays.ch arrays_read arrays_write 
all: clean default

gread_arrays.ch gwrite_arrays.ch: arrays.xml
	${ADIOS_DIR}/bin/gpp.py arrays.xml

arrays_read: arrays_read.o 
	${CC} ${CFLAGS} -o arrays_read $^ ${ADIOS_LIB} 

arrays_write: arrays_write.o
	${CC} ${CFLAGS} -o arrays_write $^ ${ADIOS_LIB}

clean:
	rm -f *.o *.mod *.fh core.* *.ch
	rm -f arrays_write arrays_read 

distclean: clean
	rm -f fort.* 
	rm -f *.png minmax 
	rm -rf *.bp *.bp.dir *.idx
	rm -f *.h5
	rm -f conf

