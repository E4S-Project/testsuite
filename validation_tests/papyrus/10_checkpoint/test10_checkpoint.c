#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <papyrus/kv.h>
#include <papyrus/mpi.h>
#include <unistd.h>

int rank, size, peer;
char name[256];
int db;
int ret;
int event;

const char* k[] = { "GOOGLE", "FACEBOOK", "TWITTER", "JUNGWONKIM" };
const char* v[] = { "https://google.com", "https://facebook.com", "https://twitter.com", "http://jungwon.kim" };


int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    papyruskv_init(&argc, &argv, "kv_repo");

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Get_processor_name(name, &ret);

    peer = rank == size - 1 ? 0 : rank + 1;

    printf("[%s:%d] [%s] [%d/%d]\n", __FILE__, __LINE__, name, rank, size);

    ret = papyruskv_open("TEST_DB", PAPYRUSKV_CREATE | PAPYRUSKV_RELAXED | PAPYRUSKV_RDWR, NULL, &db);
    if (ret != PAPYRUSKV_OK) printf("[%s:%d] FAILED:ret[%d]\n", __FILE__, __LINE__, ret);
    printf("[%s:%d] db[%d]\n", __FILE__, __LINE__, db);

    if (rank < sizeof(k) / sizeof(char*)) {
        ret = papyruskv_put(db, k[rank], strlen(k[rank]) + 1, v[rank], strlen(v[rank]) + 1);
        if (ret != PAPYRUSKV_OK) printf("[%s:%d] FAILED:ret[%d]\n", __FILE__, __LINE__, ret);

        printf("[%s:%d] PUT:rank[%d] key[%s] value[%s]\n", __FILE__, __LINE__, rank, k[rank], v[rank]);
    }

    ret = papyruskv_barrier(db, PAPYRUSKV_MEMTABLE);
    if (ret != PAPYRUSKV_OK) printf("[%s:%d] FAILED:ret[%d]\n", __FILE__, __LINE__, ret);
    printf("[%s:%d] BARRIER:rank[%d]\n", __FILE__, __LINE__, rank);

    if (peer < sizeof(k) / sizeof(char*)) {
        char* val = NULL;
        size_t vallen = 0UL;
        ret = papyruskv_get(db, k[peer], strlen(k[peer]) + 1, &val, &vallen);
        if (ret != PAPYRUSKV_OK) printf("[%s:%d] FAILED:rank[%d] peer[%d] key[%s] value[%s] vallen[%lu]\n", __FILE__, __LINE__, rank, peer, k[peer], val, vallen);
        else printf("[%s:%d] GET:rank[%d] peer[%d] key[%s] value[%s] vallen[%lu]\n", __FILE__, __LINE__, rank, peer, k[peer], val, vallen);
    }

    ret = papyruskv_checkpoint(db, "./kv_checkpoint", &event);
    if (ret != PAPYRUSKV_OK) printf("[%s:%d] FAILED:ret[%d]\n", __FILE__, __LINE__, ret);
    printf("[%s:%d] CHECKPOINT [%s] start\n", __FILE__, __LINE__, "./kv_checkpoint");

    ret = papyruskv_wait(db, event);
    if (ret != PAPYRUSKV_OK) printf("[%s:%d] FAILED:ret[%d]\n", __FILE__, __LINE__, ret);
    printf("[%s:%d] CHECKPOINT [%s] finish\n", __FILE__, __LINE__, "./kv_checkpoint");

    ret = papyruskv_close(db);
    if (ret != PAPYRUSKV_OK) printf("[%s:%d] FAILED:ret[%d]\n", __FILE__, __LINE__, ret);

    papyruskv_finalize();
    MPI_Finalize();
    return 0;
}

