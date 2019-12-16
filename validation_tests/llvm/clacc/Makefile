CC=clang
CCOPT= -g -O3 -Wall

all: householder

householder: householder.c
	$(CC) $(CCOPT) -fopenacc -o $@ $< -lm

clean:
	rm -f householder *.o

