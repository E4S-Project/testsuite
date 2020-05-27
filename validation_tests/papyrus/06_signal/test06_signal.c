#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <papyrus/kv.h>
#include <papyrus/mpi.h>
#include <unistd.h>

int rank, size;
char name[256];
int db;
int ret;
int i;

const char* k[] = { "GOOGLE", "FACEBOOK", "TWITTER", "JUNGWONKIM" };
const char* v[] = { "https://google.com", "https://facebook.com", "https://twitter.com", "http://jungwon.kim" };


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

    ret = papyruskv_consistency(db, PAPYRUSKV_SEQUENTIAL);
    if (ret != PAPYRUSKV_OK) printf("[%s:%d] FAILED:ret[%d]\n", __FILE__, __LINE__, ret);
    printf("[%s:%d] db[%d] consistency[0x%x]\n", __FILE__, __LINE__, db, PAPYRUSKV_SEQUENTIAL);

    if (rank == 0) {
        for (i = 0; i < sizeof(k) / sizeof(char*); i++) {
            ret = papyruskv_put(db, k[i], strlen(k[i]) + 1, v[i], strlen(v[i]) + 1);
            if (ret != PAPYRUSKV_OK) printf("[%s:%d] FAILED:ret[%d]\n", __FILE__, __LINE__, ret);

            printf("[%s:%d] PUT:rank[%d] key[%s] value[%s]\n", __FILE__, __LINE__, rank, k[i], v[i]);
        }
        int ranks[3] = { 1, 2, 3 };
        papyruskv_signal_notify(0xbeef, ranks, 3);
    } else {
        int ranks[1] = { 0 };
        papyruskv_signal_wait(0xbeef, ranks, 1);
    }

    if (rank < sizeof(k) / sizeof(char*)) {
        char* val = NULL;
        size_t vallen = 0UL;
        ret = papyruskv_get(db, k[rank], strlen(k[rank]) + 1, &val, &vallen);
        if (ret != PAPYRUSKV_OK) printf("FAILED:[%s:%d] ret[%d] rank[%d]\n", __FILE__, __LINE__, ret, rank);
        else printf("[%s:%d] GET:rank[%d] key[%s] value[%s] vallen[%lu]\n", __FILE__, __LINE__, rank, k[rank], val, vallen);
    }

    ret = papyruskv_close(db);
    if (ret != PAPYRUSKV_OK) printf("[%s:%d] FAILED:ret[%d]\n", __FILE__, __LINE__, ret);

    papyruskv_finalize();
    MPI_Finalize();
    return 0;
}

