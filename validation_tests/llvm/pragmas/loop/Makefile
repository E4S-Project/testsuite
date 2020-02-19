CC=clang
CCOPT=  -mllvm -polly -mllvm -polly-process-unprofitable -Wall 

#all: matmul householder householder2
all: matmul_paper

matmul_paper: matmul_paper.c
	$(CC) $(CCOPT) -o $@ $< -lm

householder: householder.c
	$(CC) $(CCOPT) -o $@ $< -lm

householder2: householder2.c
	$(CC) $(CCOPT) -o $@ $< -lm

clean:
	rm -f matmul_paper householder householder2 *.o
