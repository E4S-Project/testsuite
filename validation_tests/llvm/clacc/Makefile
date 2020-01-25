CC=clang
CCOPT= -g -O3 
ACC=-fopenacc

all: householder

householder: householder.c
	$(CC) $(CCOPT) $(ACC) -o $@ $< -lm

householder2: householder2.c
	$(CC) $(CCOPT) $(ACC) -o $@ $< -lm

clean:
	rm -f householder *.o

