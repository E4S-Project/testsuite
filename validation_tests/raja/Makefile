objects = ex1_vector-addition_solution ex2_approx-pi_solution ex3_colored-indexset_solution ex5_line-of-sight_solution ex6_stencil-offset-layout_solution ex7_nested-loop-reorder_solution ex8_tiled-matrix-transpose_solution ex9_matrix-transpose-local-array_solution #ex4_atomic-histogram_solution
all: $(objects)
CXX	=$(TEST_CXX) #g++
CXXFLAGS= -fopenmp -O3 -g -I$(RAJA_ROOT)/include -L$(RAJA_ROOT)/lib

$(objects): %: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< -lRAJA

clean:
	rm -f *.o $(objects)
