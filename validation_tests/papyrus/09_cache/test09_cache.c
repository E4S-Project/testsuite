#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <papyrus/kv.h>
#include <papyrus/mpi.h>
#include <unistd.h>

int rank, size, peer;
char name[256];
int db;
int prot;
int ret;

const char* k[] = { "GOOGLE", "FACEBOOK", "TWITTER", "JUNGWONKIM" };
const char* v1[] = { "https://google.com", "https://facebook.com", "https://twitter.com", "http://jungwon.kim" };
const char* v2[] = { "HTTPS://GOOGLE.COM", "HTTPS://FACEBOOK.COM", "HTTPS://TWITTER.COM", "HTTP://JUNGWON.KIM" };


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
        ret = papyruskv_put(db, k[rank], strlen(k[rank]) + 1, v1[rank], strlen(v1[rank]) + 1);
        if (ret != PAPYRUSKV_OK) printf("[%s:%d] FAILED:ret[%d]\n", __FILE__, __LINE__, ret);

        printf("[%s:%d] PUT:rank[%d] key[%s] value[%s]\n", __FILE__, __LINE__, rank, k[rank], v1[rank]);
    }

    prot = PAPYRUSKV_RDONLY;
    ret = papyruskv_protect(db, prot);
    if (ret != PAPYRUSKV_OK) printf("[%s:%d] FAILED:ret[%d]\n", __FILE__, __LINE__, ret);
    printf("[%s:%d] PROTECT:rank[%d] prot[%x]\n", __FILE__, __LINE__, rank, prot);

    if (peer < sizeof(k) / sizeof(char*)) {
        char* val = NULL;
        size_t vallen = 0UL;
        ret = papyruskv_get(db, k[peer], strlen(k[peer]) + 1, &val, &vallen);
        if (ret != PAPYRUSKV_OK) printf("[%s:%d] FAILED:rank[%d] peer[%d] key[%s] value[%s] vallen[%lu]\n", __FILE__, __LINE__, rank, peer, k[peer], val, vallen);
        else printf("[%s:%d] GET:rank[%d] peer[%d] key[%s] value[%s] vallen[%lu]\n", __FILE__, __LINE__, rank, peer, k[peer], val, vallen);
    }

    if (peer < sizeof(k) / sizeof(char*)) {
        char* val = NULL;
        size_t vallen = 0UL;
        ret = papyruskv_get(db, k[peer], strlen(k[peer]) + 1, &val, &vallen);
        if (ret != PAPYRUSKV_OK) printf("[%s:%d] FAILED:rank[%d] peer[%d] key[%s] value[%s] vallen[%lu]\n", __FILE__, __LINE__, rank, peer, k[peer], val, vallen);
        else printf("[%s:%d] GET:rank[%d] peer[%d] key[%s] value[%s] vallen[%lu]\n", __FILE__, __LINE__, rank, peer, k[peer], val, vallen);
    }

    prot = PAPYRUSKV_WRONLY;
    ret = papyruskv_protect(db, prot);
    if (ret != PAPYRUSKV_OK) printf("[%s:%d] FAILED:ret[%d]\n", __FILE__, __LINE__, ret);
    printf("[%s:%d] PROTECT:rank[%d] prot[%x]\n", __FILE__, __LINE__, rank, prot);

    if (rank < sizeof(k) / sizeof(char*)) {
        ret = papyruskv_put(db, k[rank], strlen(k[rank]) + 1, v2[rank], strlen(v2[rank]) + 1);
        if (ret != PAPYRUSKV_OK) printf("[%s:%d] FAILED:ret[%d]\n", __FILE__, __LINE__, ret);

        printf("[%s:%d] PUT:rank[%d] key[%s] value[%s]\n", __FILE__, __LINE__, rank, k[rank], v2[rank]);
    }

    prot = PAPYRUSKV_RDONLY;
    ret = papyruskv_protect(db, prot);
    if (ret != PAPYRUSKV_OK) printf("[%s:%d] FAILED:ret[%d]\n", __FILE__, __LINE__, ret);
    printf("[%s:%d] PROTECT:rank[%d] prot[%x]\n", __FILE__, __LINE__, rank, prot);

    if (peer < sizeof(k) / sizeof(char*)) {
        char* val = NULL;
        size_t vallen = 0UL;
        ret = papyruskv_get(db, k[peer], strlen(k[peer]) + 1, &val, &vallen);
        if (ret != PAPYRUSKV_OK) printf("[%s:%d] FAILED:rank[%d] peer[%d] key[%s] value[%s] vallen[%lu]\n", __FILE__, __LINE__, rank, peer, k[peer], val, vallen);
        else printf("[%s:%d] GET:rank[%d] peer[%d] key[%s] value[%s] vallen[%lu]\n", __FILE__, __LINE__, rank, peer, k[peer], val, vallen);
    }

    if (peer < sizeof(k) / sizeof(char*)) {
        char* val = NULL;
        size_t vallen = 0UL;
        ret = papyruskv_get(db, k[peer], strlen(k[peer]) + 1, &val, &vallen);
        if (ret != PAPYRUSKV_OK) printf("[%s:%d] FAILED:rank[%d] peer[%d] key[%s] value[%s] vallen[%lu]\n", __FILE__, __LINE__, rank, peer, k[peer], val, vallen);
        else printf("[%s:%d] GET:rank[%d] peer[%d] key[%s] value[%s] vallen[%lu]\n", __FILE__, __LINE__, rank, peer, k[peer], val, vallen);
    }

    ret = papyruskv_close(db);
    if (ret != PAPYRUSKV_OK) printf("[%s:%d] FAILED:ret[%d]\n", __FILE__, __LINE__, ret);

    papyruskv_finalize();
    MPI_Finalize();
    return 0;
}

