#include <mpi.h>
#include <stdio.h>
#include <papyrus/kv.h>
#include <papyrus/mpi.h>

int rank, size, ret;
char name[256];
int db;

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    papyruskv_init(&argc, &argv, "kv_repo");

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Get_processor_name(name, &ret);

    printf("[%s:%d] [%s] [%d/%d]\n", __FILE__, __LINE__, name, rank, size);

    ret = papyruskv_open("TEST_DB", PAPYRUSKV_CREATE | PAPYRUSKV_RELAXED | PAPYRUSKV_RDWR, NULL, &db);
    if (ret != PAPYRUSKV_OK) printf("[%s:%d] FAILED:ret[%d]\n", __FILE__, __LINE__, ret);
    printf("[%s:%d] db[%d]\n", __FILE__, __LINE__, db);

    ret = papyruskv_close(db);
    if (ret != PAPYRUSKV_OK) printf("[%s:%d] FAILED:ret[%d]\n", __FILE__, __LINE__, ret);

    papyruskv_finalize();
    MPI_Finalize();
    return 0;
}

