F18=f18
GFORTRAN=gfortran

#FC = $(GFORTRAN)
FC = $(F18)
LD = $(FC)

FCFLAGS =
LDFLAGS =

all: characters characters_wrong

characters: characters.o
	$(LD) -o $@ $< $(LDFLAGS)
	@echo -e "\033[01;32m[$@] compiled\e[0m"

characters_wrong: characters_wrong.o
	$(LD) -o $@ $< $(LDFLAGS)
	@echo -e "\033[01;32m[$@] compiled\e[0m"

%.o: %.f90
	$(FC) -o $@ -c $< $(FCFLAGS)

clean:
	rm -f *.o characters characters_wrong

