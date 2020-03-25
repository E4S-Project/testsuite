CC=clang
CCOPT= -g -O3 -std=gnu11
ACC=-fopenacc

all: householder2

householder: householder.c
	$(CC) $(CCOPT) $(ACC) -o $@ $< -lm

householder2: householder2.c
	$(CC) $(CCOPT) $(ACC) -o $@ $< -lm

clean:
	rm -f householder householder2 *.o

