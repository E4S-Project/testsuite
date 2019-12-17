# ~/x86/SOLLVE/llvm-project/install-pragma-clang-transform/bin/clang  -mllvm -polly -mllvm -polly-process-unprofitable -Wall -o matmul matmul.c


CC=clang
CCOPT=  -mllvm -polly -mllvm -polly-process-unprofitable -Wall 

all: matmul householder

matmul: matmul.c
	$(CC) $(CCOPT) -o $@ $< -lm

householder: householder.c
	$(CC) $(CCOPT) -o $@ $< -lm

clean:
	rm -f matmul householder *.o
