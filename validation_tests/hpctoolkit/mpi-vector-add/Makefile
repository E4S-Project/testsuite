# Location of the CUDA Toolkit
#### CUDA_PATH ?= /usr/local/cuda
CUDA_SPEC="cuda@11.4.0 /3zjz5ms"
CUDA_PATH=`spack location -i ${CUDA_SPEC}`

INC=-I${CUDA_PATH}/samples/common/inc
LIBS=-L${CUDA_PATH}/lib64 -Wl,-rpath=${CUDA_PATH}/lib64
NVCC=${CUDA_PATH}/bin/nvcc

OBJS=main.o vecadd.o

main: $(OBJS)
	mpicxx -o main $(OBJS) $(LIBS) -lcuda -lcudart

main.o: main.cpp
	mpicxx -c $<

vecadd.o: vecadd.cu
	${NVCC} -c $(INC) $<

run:
	eval ${TEST_RUN} hpcrun ./main > gort

clean:
	/bin/rm -rf *.o main hpctoolkit-* core.*


