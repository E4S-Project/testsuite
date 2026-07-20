#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <papi.h>

#define NUM_PTHREADS 2
#define NUM_EVENTS 2

static void do_work(void) {
    volatile double x = 1.0;
    for (long k = 0; k < 20000000; k++) {
        x += 0.000001;
    }
}

void *Thread(void *arg)
{
    int retval, i;
    long long values[NUM_EVENTS];
    int EventSet = PAPI_NULL;
    int events[NUM_EVENTS];
    char *EventName[] = { "PAPI_TOT_CYC", "PAPI_TOT_INS" };
    int thread = *(int *) arg;
    int hw_counters_active = 0;

    retval = PAPI_register_thread();
    if ( retval != PAPI_OK ) {
        printf("ERROR: PAPI_register_thread: %d: %s\n", retval, PAPI_strerror(retval));
        exit(EXIT_FAILURE);
    }

    /* Attempt HW counters for thread */
    if (PAPI_create_eventset(&EventSet) == PAPI_OK) {
        int events_ok = 1;
        for (i = 0; i < NUM_EVENTS; i++) {
            if (PAPI_event_name_to_code(EventName[i], &events[i]) != PAPI_OK) {
                events_ok = 0;
                break;
            }
        }
        if (events_ok && PAPI_add_events(EventSet, events, NUM_EVENTS) == PAPI_OK) {
            if (PAPI_start(EventSet) == PAPI_OK) {
                hw_counters_active = 1;
            }
        }
    }

    /* Software Timers per Thread */
    long long rtime_start = PAPI_get_real_usec();
    long long vtime_start = PAPI_get_virt_usec();

    do_work();

    long long rtime_end = PAPI_get_real_usec();
    long long vtime_end = PAPI_get_virt_usec();

    if (hw_counters_active) {
        PAPI_stop(EventSet, values);
        for (i = 0; i < NUM_EVENTS; i++) {
            printf("[Thread %d] HW %s: %12lld\n", thread, EventName[i], values[i]);
        }
    }

    long long elapsed_rtime = rtime_end - rtime_start;
    long long elapsed_vtime = vtime_end - vtime_start;

    printf("[Thread %d] Real Time: %lld us | Virtual CPU Time: %lld us\n",
           thread, elapsed_rtime, elapsed_vtime);

    if (elapsed_rtime <= 0 || elapsed_vtime <= 0) {
        printf("ERROR: Thread %d timer returned invalid measurements!\n", thread);
        exit(EXIT_FAILURE);
    }

    PAPI_unregister_thread();
    return NULL;
}

int main( int argc, char **argv )
{
    pthread_t tids[NUM_PTHREADS];
    int i, vals[NUM_PTHREADS];
    int retval;

    retval = PAPI_library_init( PAPI_VER_CURRENT );
    if ( retval != PAPI_VER_CURRENT ) {
        printf("ERROR: PAPI_library_init: %d: %s\n", retval, PAPI_strerror(retval) );
        exit(EXIT_FAILURE);
    }

    retval = PAPI_thread_init((unsigned long (*)(void))(pthread_self));
    if ( retval != PAPI_OK ) {
        printf("ERROR: PAPI_thread_init: %d: %s\n", retval, PAPI_strerror(retval) );
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < NUM_PTHREADS; i++) {
        vals[i] = i;
        pthread_create(&tids[i], NULL, Thread, &vals[i]);
    }

    for (i = 0; i < NUM_PTHREADS; i++) {
        pthread_join(tids[i], NULL);
    }

    PAPI_shutdown();
    printf("Multi-thread software timer validation PASSED.\n");
    return EXIT_SUCCESS;
}
