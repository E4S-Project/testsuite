#include <stdio.h>
#include <papi.h>
#include "matmul.h"

int main(int argc, char *argv[])
{
    int papi_errno;
    hipError_t hip_errno;

    papi_errno = PAPI_library_init(PAPI_VER_CURRENT);
    if (papi_errno != PAPI_VER_CURRENT) {
        fprintf(stderr, "ERROR: PAPI_library_init: runtime lib ver %d not equal to %d\n", papi_errno, PAPI_VER_CURRENT);
        exit(EXIT_FAILURE);
    }

#define NUM_EVENTS (3)
    const char *events[NUM_EVENTS] = {
        "rocm:::SQ_INSTS_VALU",
        "rocm:::SQ_WAVES",
        "rocm:::SQ_WAVES_RESTORED",
    };

    int eventset = PAPI_NULL;
    papi_errno = PAPI_create_eventset(&eventset);
    if (papi_errno != PAPI_OK) {
        fprintf(stderr, "ERROR: PAPI_create_eventset: %d: %s\n", papi_errno, PAPI_strerror(papi_errno));
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < NUM_EVENTS; ++i) {
        char named_event[PAPI_MAX_STR_LEN] = { 0 };
        sprintf(named_event, "%s:device=0", events[i]);
        papi_errno = PAPI_add_named_event(eventset, named_event);
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

    hipStream_t stream;
    hip_errno = hipStreamCreate(&stream);
    if (hip_errno != hipSuccess) {
        fprintf(stderr, "ERROR: hipStreamCreate: %d: %s\n", PAPI_EMISC, PAPI_strerror(PAPI_EMISC));
        exit(EXIT_FAILURE);
    }

    int matmul_errno;
    void *handle;
    matmul_errno = matmul_init(&handle);
    if (matmul_errno != MATMUL_SUCCESS) {
        fprintf(stderr, "ERROR: matmul_init: %d: %s\n", PAPI_EMISC, PAPI_strerror(PAPI_EMISC));
        exit(EXIT_FAILURE);
    }

#define KERNEL_BATCH_SIZE (2)
    for (int i = 0; i < KERNEL_BATCH_SIZE; ++i) {
        matmul_errno = matmul_run(handle, stream);
        if (matmul_errno != MATMUL_SUCCESS) {
            fprintf(stderr, "ERROR: matmul_run: %d: %s\n", PAPI_EMISC, PAPI_strerror(PAPI_EMISC));
            exit(EXIT_FAILURE);
        }
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
        fprintf(stdout, "%s:device=0 : %lld\n", events[i], counters[i]);
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

    PAPI_shutdown();
    return EXIT_SUCCESS;
}
