#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <omp.h>
#include "papi.h"
#include "matmul.h"

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

    papi_errno = PAPI_thread_init((unsigned long (*)(void)) omp_get_thread_num);
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
    omp_set_num_threads(num_threads);
    fprintf(stdout, "Run rocm test with %d threads\n", num_threads);

#define NUM_EVENTS 2
    const char *events[NUM_EVENTS] = {
        "rocm:::SQ_WAVES",
        "rocm:::SQ_WAVES_RESTORED",
    };

#pragma omp parallel
    {
        int eventset = PAPI_NULL;
        papi_errno = PAPI_create_eventset(&eventset);
        if (papi_errno != PAPI_OK) {
            fprintf(stderr, "ERROR: PAPI_create_eventset: %d: %s\n", papi_errno, PAPI_strerror(papi_errno));
            exit(EXIT_FAILURE);
        }

        int thread_num = omp_get_thread_num();
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

        hip_errno = hipSetDevice(thread_num);
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
                    omp_get_thread_num(), events[i], thread_num,
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
    }

    PAPI_shutdown();

    return EXIT_SUCCESS;
}
