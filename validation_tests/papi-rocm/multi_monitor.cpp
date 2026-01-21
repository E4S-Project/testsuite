#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "papi.h"
#include "matmul.h"

#define NUM_EVENTS 2
const char *events[NUM_EVENTS] = {
    "rocm:::SQ_WAVES",
    "rocm:::SQ_WAVES_RESTORED",
};

typedef struct {
    int num_thread;
} thread_arg_t;

void *run(void *arg)
{
    int eventset = PAPI_NULL;
    int papi_errno = PAPI_create_eventset(&eventset);
    if (papi_errno != PAPI_OK) {
        fprintf(stderr, "ERROR: PAPI_create_eventset: %d: %s\n", papi_errno, PAPI_strerror(papi_errno));
        exit(EXIT_FAILURE);
    }

    int thread_num = ((thread_arg_t *) arg)->num_thread;
    for (int j = 0; j < NUM_EVENTS; ++j) {
        char named_event[PAPI_MAX_STR_LEN] = { 0 };
        sprintf(named_event, "%s:device=%d", events[j], thread_num);
        papi_errno = PAPI_add_named_event(eventset, (const char *) named_event);
        if (papi_errno != PAPI_OK && papi_errno != PAPI_ENOEVNT) {
            fprintf(stderr, "ERROR: PAPI_add_named_event: %d: %s\n", papi_errno, PAPI_strerror(papi_errno));
            exit(EXIT_FAILURE);
        }
    }

    papi_errno = PAPI_start(eventset);
    if (papi_errno != PAPI_OK) {
        fprintf(stderr, "ERROR: PAPI_start: %d: %s\n", papi_errno, PAPI_strerror(papi_errno));
        exit(EXIT_FAILURE);
    }

    hipError_t hip_errno = hipSetDevice(thread_num);
    if (hip_errno != hipSuccess) {
        fprintf(stderr, "ERROR: hipSetDevice: %d: %s\n", PAPI_EMISC, PAPI_strerror(PAPI_EMISC));
        exit(EXIT_FAILURE);
    }

    hipStream_t stream;
    hip_errno = hipStreamCreate(&stream);
    if (hip_errno != hipSuccess) {
        fprintf(stderr, "ERROR: hipStreamCreate: %d: %s\n", PAPI_EMISC, PAPI_strerror(PAPI_EMISC));
        exit(EXIT_FAILURE);
    }

    void *handle;
    int matmul_errno;
    matmul_errno = matmul_init(&handle);
    if (matmul_errno != MATMUL_SUCCESS) {
        fprintf(stderr, "ERROR: matmul_init: %d: %s\n", PAPI_EMISC, PAPI_strerror(PAPI_EMISC));
        exit(EXIT_FAILURE);
    }

    matmul_errno = matmul_run(handle, stream);
    if (matmul_errno != MATMUL_SUCCESS) {
        fprintf(stderr, "ERROR: matmul_run: %d: %s\n", PAPI_EMISC, PAPI_strerror(PAPI_EMISC));
        exit(EXIT_FAILURE);
    }

    hip_errno = hipStreamSynchronize(stream);
    if (hip_errno != hipSuccess) {
        fprintf(stderr, "ERROR: hipStreamSynchronize: %d: %s\n", PAPI_EMISC, PAPI_strerror(PAPI_EMISC));
        exit(EXIT_FAILURE);
    }

    hip_errno = hipStreamDestroy(stream);
    if (hip_errno != hipSuccess) {
        fprintf(stderr, "ERROR: hipStreamDestroy: %d: %s\n", PAPI_EMISC, PAPI_strerror(PAPI_EMISC));
        exit(EXIT_FAILURE);
    }

    matmul_errno = matmul_finalize(&handle);
    if (matmul_errno != MATMUL_SUCCESS) {
        fprintf(stderr, "ERROR: matmul_finalize: %d: %s\n", PAPI_EMISC, PAPI_strerror(PAPI_EMISC));
        exit(EXIT_FAILURE);
    }

    long long counters[NUM_EVENTS] = { 0 };
    papi_errno = PAPI_stop(eventset, counters);
    if (papi_errno != PAPI_OK) {
        fprintf(stderr, "ERROR: PAPI_stop: %d: %s\n", papi_errno, PAPI_strerror(papi_errno));
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < NUM_EVENTS; ++i) {
        fprintf(stdout, "[tid:%d] %s:device=%d : %lld\n",
                thread_num, events[i], thread_num,
                counters[i]);
    }

    papi_errno = PAPI_cleanup_eventset(eventset);
    if (papi_errno != PAPI_OK) {
        fprintf(stderr, "ERROR: PAPI_cleanup_eventset: %d: %s\n", papi_errno, PAPI_strerror(papi_errno));
        exit(EXIT_FAILURE);
    }

    papi_errno = PAPI_destroy_eventset(&eventset);
    if (papi_errno != PAPI_OK) {
        fprintf(stderr, "ERROR: PAPI_destroy_eventset: %d: %s\n", papi_errno, PAPI_strerror(papi_errno));
        exit(EXIT_FAILURE);
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    int papi_errno;
    hipError_t hip_errno;

    int num_threads = 1;
    if (argc > 1) {
        if (strncmp(argv[1], "--threads=", strlen("--threads=")) == 0) {
            num_threads = (int) strtol(argv[1] + strlen("--threads="), NULL, 10);
        } else if (strcmp(argv[1], "--help") == 0) {
            fprintf(stdout, "Usage %s [OPTIONS]\n", argv[0]);
            fprintf(stdout, "[OPTIONS]\n");
            fprintf(stdout, "     --help\n");
            fprintf(stdout, "     --threads=[N]\n");
            exit(EXIT_FAILURE);
        }
    }

    papi_errno = PAPI_library_init(PAPI_VER_CURRENT);
    if (papi_errno != PAPI_VER_CURRENT) {
        fprintf(stderr, "ERROR: PAPI_library_init: runtime lib ver %d not equal to %d\n", papi_errno, PAPI_VER_CURRENT);
        exit(EXIT_FAILURE);
    }

    papi_errno = PAPI_thread_init((unsigned long (*)(void)) pthread_self);
    if (papi_errno != PAPI_OK) {
        fprintf(stderr, "ERROR: PAPI_thread_init: %d: %s\n", papi_errno, PAPI_strerror(papi_errno));
        exit(EXIT_FAILURE);
    }

    int num_devices;
    hip_errno = hipGetDeviceCount(&num_devices);
    if (hip_errno != hipSuccess) {
        fprintf(stderr, "ERROR: hipGetDeviceCount: %d: %s\n", PAPI_EMISC, PAPI_strerror(PAPI_EMISC));
        exit(EXIT_FAILURE);
    }

    num_threads = (num_threads < num_devices) ? num_threads : num_devices;
    fprintf(stdout, "Run rocm test with %d threads\n", num_threads);

    pthread_t *thread = (pthread_t *)malloc(num_threads * sizeof(*thread));
    if (thread == NULL) {
        return EXIT_FAILURE;
    }

    thread_arg_t *arg = (thread_arg_t *)malloc(num_threads * sizeof(*arg));
    if (arg == NULL) {
        return EXIT_FAILURE;
    }

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for (int i = 0; i < num_threads; ++i) {
        arg[i].num_thread = i;
        pthread_create(&thread[i], &attr, run, &arg[i]);
    }

    for (int i = 0; i < num_threads; ++i) {
        pthread_join(thread[i], NULL);
    }

    free(thread);
    free(arg);
    PAPI_shutdown();

    return EXIT_SUCCESS;
}
