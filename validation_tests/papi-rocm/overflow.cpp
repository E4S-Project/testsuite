#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <papi.h>
#include "matmul.h"

#define EV_THRESHOLD 1000

int setup_PAPI(int *event_set, int threshold);
int unset_PAPI(int event_set);

int remaining_handler_invocations = 10;

int main(int argc, char **argv)
{
    int papi_errno;
    hipError_t hip_errno;
    int event_set = PAPI_NULL;
    long long counter_values[1] = { 0 };

    /* ROCP_HSA_INTERCEPT can't be set to 1 when using PAPI_overflow */
    setenv("ROCP_HSA_INTERCEPT", "0", 1);

    papi_errno = setup_PAPI(&event_set, EV_THRESHOLD);
    if (papi_errno != PAPI_OK) {
        exit(EXIT_FAILURE);
    }

    int matmul_errno;
    void *handler;
    matmul_errno = matmul_init(&handler);
    if (matmul_errno != MATMUL_SUCCESS) {
        fprintf(stderr, "ERROR: matmul_init: %d: %s\n", PAPI_EMISC, PAPI_strerror(PAPI_EMISC));
        exit(EXIT_FAILURE);
    }

    hipStream_t stream;
    hip_errno = hipStreamCreate(&stream);
    if (hip_errno != hipSuccess) {
        fprintf(stderr, "ERROR: hipStreamCreate: %d: %s\n", PAPI_EMISC, PAPI_strerror(PAPI_EMISC));
        exit(EXIT_FAILURE);
    }

    papi_errno = PAPI_start(event_set);
    if (papi_errno != PAPI_OK) {
        fprintf(stderr, "ERROR: PAPI_start: %d: %s\n", papi_errno, PAPI_strerror(papi_errno));
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < 5; i++) {
        matmul_errno = matmul_run(handler, stream);
        if (matmul_errno != MATMUL_SUCCESS) {
            fprintf(stderr, "ERROR: matmul_run: %d: %s\n", PAPI_EMISC, PAPI_strerror(PAPI_EMISC));
            exit(EXIT_FAILURE);
        }

        papi_errno = PAPI_accum(event_set, counter_values);
        if (papi_errno != PAPI_OK) {
            fprintf(stderr, "ERROR: PAPI_accum: %d: %s\n", papi_errno, PAPI_strerror(papi_errno));
            exit(EXIT_FAILURE);
        }

        fprintf(stdout, "rocm:::SQ_WAVES:device=0 : %lld\n", counter_values[0]);
        counter_values[0] = 0;

        matmul_errno = matmul_run(handler, 0);
        if (matmul_errno != MATMUL_SUCCESS) {
            fprintf(stderr, "ERROR: matmul_run: %d: %s\n", PAPI_EMISC, PAPI_strerror(PAPI_EMISC));
            exit(EXIT_FAILURE);
        }

        papi_errno = PAPI_accum(event_set, counter_values);
        if (papi_errno != PAPI_OK) {
            fprintf(stderr, "ERROR: PAPI_accum: %d: %s\n", papi_errno, PAPI_strerror(papi_errno));
            exit(EXIT_FAILURE);
        }

        fprintf(stdout, "rocm:::SQ_WAVES:device=0 : %lld\n", counter_values[0]);
        counter_values[0] = 0;
    }

    papi_errno = PAPI_stop(event_set, counter_values);
    if (papi_errno != PAPI_OK) {
        fprintf(stderr, "ERROR: PAPI_stop: %d: %s\n", papi_errno, PAPI_strerror(papi_errno));
        exit(EXIT_FAILURE);
    }

    unset_PAPI(event_set);
    matmul_errno = matmul_finalize(&handler);
    if (matmul_errno != MATMUL_SUCCESS) {
        fprintf(stderr, "ERROR: matmul_finalize: %d: %s\n", PAPI_EMISC, PAPI_strerror(PAPI_EMISC));
        exit(EXIT_FAILURE);
    }

    hip_errno = hipStreamDestroy(stream);
    if (hip_errno != hipSuccess) {
        fprintf(stderr, "ERROR: hipStreamDestroy: %d: %s\n", PAPI_EMISC, PAPI_strerror(PAPI_EMISC));
        exit(EXIT_FAILURE);
    }

    if (remaining_handler_invocations > 1) {
        fprintf(stderr, "ERROR: ROCm overflow handler was not invoked as expected!\n");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}

void overflow_handler(int event_set, void *address __attribute__((unused)),
                      long long overflow_vector,
                      void *context __attribute__((unused)))
{
    int papi_errno;
    char event_name[PAPI_MAX_STR_LEN];
    int *event_codes, event_index, number=1;

    papi_errno = PAPI_get_overflow_event_index(event_set, overflow_vector, &event_index, &number);
    if (papi_errno != PAPI_OK) {
        fprintf(stderr, "ERROR: PAPI_overflow_event_index: %d: %s\n", papi_errno, PAPI_strerror(papi_errno));
        exit(EXIT_FAILURE);
    }

    number = event_index + 1;
    event_codes = (int *) calloc(number, sizeof(int));

    papi_errno = PAPI_list_events(event_set, event_codes, &number);
    if (papi_errno != PAPI_OK) {
        fprintf(stderr, "ERROR: PAPI_list_events: %d: %s\n", papi_errno, PAPI_strerror(papi_errno));
        exit(EXIT_FAILURE);
    }

    papi_errno = PAPI_event_code_to_name(event_codes[event_index], event_name);
    if (papi_errno != PAPI_OK) {
        fprintf(stderr, "ERROR: PAPI_event_code_to_name: %d: %s\n", papi_errno, PAPI_strerror(papi_errno));
        exit(EXIT_FAILURE);
    }

    free(event_codes);
    fprintf(stdout, "Event \"%s\" at index: %d exceeded its threshold again.\n", event_name, event_index);

    if (!strcmp(event_name, "rocm:::SQ_WAVES:device=0") || !event_index) {
        remaining_handler_invocations--;
    }

    return;
}

int setup_PAPI(int *event_set, int threshold)
{
    int papi_errno;
    int event_code;

    papi_errno = PAPI_library_init(PAPI_VER_CURRENT);
    if (papi_errno != PAPI_VER_CURRENT) {
        fprintf(stderr, "ERROR: PAPI_library_init: runtime lib ver %d not equal to %d\n", papi_errno, PAPI_VER_CURRENT);
        return papi_errno;
    }

    papi_errno = PAPI_create_eventset(event_set);
    if (papi_errno != PAPI_OK) {
        fprintf(stderr, "ERROR: PAPI_create_eventset: %d: %s\n", papi_errno, PAPI_strerror(papi_errno));
        return papi_errno;
    }

    papi_errno = PAPI_event_name_to_code("rocm:::SQ_WAVES:device=0", &event_code);
    if (papi_errno != PAPI_OK) {
        fprintf(stderr, "ERROR: PAPI_event_name_to_code: %d: %s\n", papi_errno, PAPI_strerror(papi_errno));
        return papi_errno;
    }

    papi_errno = PAPI_add_event(*event_set, event_code);
    if (papi_errno != PAPI_OK) {
        fprintf(stderr, "ERROR: PAPI_add_event: %d: %s\n", papi_errno, PAPI_strerror(papi_errno));
        return papi_errno;
    }

    papi_errno = PAPI_overflow(*event_set, event_code, threshold, PAPI_OVERFLOW_FORCE_SW, overflow_handler);
    if (papi_errno != PAPI_OK) {
        fprintf(stderr, "ERROR: PAPI_overflow: %d: %s\n", papi_errno, PAPI_strerror(papi_errno));
        return papi_errno;
    }

    return papi_errno;
}

int unset_PAPI(int event_set)
{
    int papi_errno;
    int event_code;

    papi_errno = PAPI_event_name_to_code("rocm:::SQ_WAVES:device=0", &event_code);
    if (papi_errno != PAPI_OK) {
        fprintf(stderr, "ERROR: PAPI_event_name_to_code: %d: %s\n", papi_errno, PAPI_strerror(papi_errno));
        return papi_errno;
    }

    papi_errno = PAPI_overflow(event_set, event_code, 0, PAPI_OVERFLOW_FORCE_SW, overflow_handler);
    if (papi_errno != PAPI_OK) {
        fprintf(stderr, "ERROR: PAPI_overflow: %d: %s\n", papi_errno, PAPI_strerror(papi_errno));
        return papi_errno;
    }

    return papi_errno;
}
