## * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
##   Makefile to test SZ library.

##   PLEASE SET THIS VARIABLE BEFORE COMPILING
SZPATH		= $(SZ_ROOT)
#GSLPATH = /usr/local/Cellar/gsl/2.4
##   COMPILERS
CC		= $(TEST_CC) #gcc
FT		= $(TEST_FTN) #gfortran
MPICC 		= $(TEST_CC_MPI) #mpicc
MPIRUN 		= $(TEST_RUN_CMD) #mpirun
HDF5CC		= h5cc

##   FLAGS
# Compiling using shared library
#SZFLAG 	= -I$(SZPATH)/include -L$(SZPATH)/lib -lSZ
# Compiling using static library
SZFLAG 	= -I$(SZPATH)/include/ -I$(SZPATH)/include/sz  $(SZ_LIB_PATH)/libSZ.so -lm -Wl,-rpath,$(SZ_LIB_PATH)
GSLFLAG = -I$(GSLPATH)/include $(GSLPATH)/lib/libgsl.a
NETCDFFLAG = -I$(NETCDFPATH)/include -lnetcdf


##   TARGETS
all: 		sz testint_compress testint_decompress testfloat_compress testfloat_decompress testdouble_compress testdouble_decompress
#all: 		testdouble_compress_f testdouble_decompress_f testdouble_batch_compress_f

#testfloat_compress_ts:	testfloat_compress_ts.c
#	$(CC)  -g -O3 -o testfloat_compress_ts testfloat_compress_ts.c $(SZFLAG)

testint_compress:	testint_compress.c
	$(CC)  -g -O3 -o testint_compress testint_compress.c $(SZFLAG)

testint_decompress:	testint_decompress.c
	$(CC)  -g -O3 -o testint_decompress testint_decompress.c $(SZFLAG)

testfloat_compress:	testfloat_compress.c
	$(CC)  -g -O3 -o testfloat_compress testfloat_compress.c $(SZFLAG)

testfloat_decompress:	testfloat_decompress.c
	$(CC)  -g -O3 -o testfloat_decompress testfloat_decompress.c $(SZFLAG)

testdouble_compress:	testdouble_compress.c
	$(CC)  -g -O3 -o testdouble_compress testdouble_compress.c $(SZFLAG)

testdouble_decompress:	testdouble_decompress.c
	$(CC)  -g -O3 -o testdouble_decompress testdouble_decompress.c $(SZFLAG)

testdouble_compress_f:      testdouble_compress_f.f90
	$(FT)  -g -O3 -o testdouble_compress_f testdouble_compress_f.f90 $(SZFLAG)

testdouble_decompress_f:      testdouble_decompress_f.f90
	$(FT)  -g -O3 -o testdouble_decompress_f testdouble_decompress_f.f90 $(SZFLAG)

sz:		sz.c
	$(CC)  -g -O3 -o sz	sz.c $(SZFLAG)

clean:
	rm -f test*_compress test*_decompress test*_compress_f test*_decompress_f sz

