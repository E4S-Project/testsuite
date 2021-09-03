#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_DEVICE -1

extern int vecadd(int device);

int
main(int argc, char **argv)
{
   int myrank;

   int device = DEFAULT_DEVICE;

   int divisor = atoi(argv[1]);

   MPI_Init(&argc, &argv);

   MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

   MPI_Bcast(&divisor, 1, MPI_INT, 0, MPI_COMM_WORLD);

   printf("Rank %d begin\n", myrank);

   if (divisor > 0) device = myrank / divisor;
   
   vecadd(device);

   printf("Rank %d complete\n", myrank);

   MPI_Finalize();

    return 0;
}

