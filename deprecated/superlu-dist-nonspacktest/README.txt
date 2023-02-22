Superlu-Dist tests taken from https://github.com/xiaoyeli/superlu_dist

EXAMPLE/pddrive.c
OMP_NUM_THREADS=2 mpiexec -n 4 ./pddrive -r 2 -c 2 g20.rua

TEST/pdtest.c
mpiexec -n 1 ./pdtest -r 1 -c 1 -s 1 -b 2 -x 8 -m 20 -f g20.rua
* parameters varied during tests
