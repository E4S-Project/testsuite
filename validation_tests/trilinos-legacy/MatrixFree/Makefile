

# Get Trilinos as one entity
include $(TRILINOS)/include/Makefile.export.Trilinos
#include $(TRILINOS)/include/Makefile.export.ML

# Make sure to use same compilers and flags as Trilinos
CXX=$(Trilinos_CXX_COMPILER)
CC=$(Trilinos_C_COMPILER)
FORT=$(Trilinos_Fortran_COMPILER)

CXX_FLAGS=$(Trilinos_CXX_COMPILER_FLAGS) $(USER_CXX_FLAGS)
C_FLAGS=$(Trilinos_C_COMPILER_FLAGS) $(USERC_FLAGS)
FORT_FLAGS=$(Trilinos_Fortran_COMPILER_FLAGS) $(USER_FORT_FLAGS)

INCLUDE_DIRS=$(Trilinos_INCLUDE_DIRS) $(Trilinos_TPL_INCLUDE_DIRS)
LIBRARY_DIRS=$(Trilinos_LIBRARY_DIRS) $(Trilinos_TPL_LIBRARY_DIRS)
LIBRARIES=$(Trilinos_LIBRARIES) $(Trilinos_TPL_LIBRARIES)

LINK_FLAGS=$(Trilinos_EXTRA_LD_FLAGS) -m64

#just assuming that epetra is turned on.
#DEFINES=-DMYAPP_EPETRA -DHAVE_MPI
#DEFINES=-DHAVE_MPI


default: print_info MatrixFree

# Echo trilinos build info just for fun
print_info:
	@echo " Found Trilinos!  Here are the details: "
	@echo "   Trilinos_VERSION = $(Trilinos_VERSION)"
	@echo "   Trilinos_PACKAGE_LIST = $(Trilinos_PACKAGE_LIST)"
	@echo "   Trilinos_LIBRARIES = $(Trilinos_LIBRARIES)"
	@echo "   Trilinos_INCLUDE_DIRS = $(Trilinos_INCLUDE_DIRS)"
	@echo "   Trilinos_LIBRARY_DIRS = $(Trilinos_LIBRARY_DIRS)"
	@echo "   Trilinos_TPL_LIST = $(Trilinos_TPL_LIST)"
	@echo "   Trilinos_TPL_INCLUDE_DIRS = $(Trilinos_TPL_INCLUDE_DIRS)"
	@echo "   Trilinos_TPL_LIBRARIES = $(Trilinos_TPL_LIBRARIES)"
	@echo "   Trilinos_TPL_LIBRARY_DIRS = $(Trilinos_TPL_LIBRARY_DIRS)"
	@echo "   Trilinos_BUILD_SHARED_LIBS = $(Trilinos_BUILD_SHARED_LIBS)"
	@echo "End of Trilinos details"

# run the given test
test: MatrixFree input.xml
	./MatrixFree

# build the 
MatrixFree: MatrixFree.o
	$(CXX) $(CXX_FLAGS) MatrixFree.o -o MatrixFree $(LINK_FLAGS) $(INCLUDE_DIRS) $(DEFINES) $(LIBRARY_DIRS) $(LIBRARIES)

MatrixFree.o:
	$(CXX) -c $(CXX_FLAGS) $(INCLUDE_DIRS) $(DEFINES) MatrixFree.cpp

.PHONY: clean
clean:
	rm -f *.o *.a *.exe 
