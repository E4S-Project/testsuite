#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <papi.h>

#define NUM_EVENTS 2

/* CPU workload so virtual CPU timers have real work to measure */
static void do_work(void) {
    volatile double x = 1.0;
    for (long k = 0; k < 20000000; k++) {
        x += 0.000001;
    }
}

int main( int argc, char **argv )
{
    int retval, i;
    long long values[NUM_EVENTS];
    int EventSet = PAPI_NULL;
    int events[NUM_EVENTS];
    char *EventName[] = { "PAPI_TOT_CYC", "PAPI_TOT_INS" };

    long long rtime_start, rtime_end;
    long long vtime_start, vtime_end;
    long long rcyc_start, rcyc_end;
    long long vcyc_start, vcyc_end;
    int hw_counters_active = 0;

    /* 1. Initialize PAPI */
    retval = PAPI_library_init( PAPI_VER_CURRENT );
    if ( retval != PAPI_VER_CURRENT ) {
        printf("ERROR: PAPI_library_init: %d: %s\n", retval, PAPI_strerror(retval));
        exit(EXIT_FAILURE);
    }

    printf("PAPI_VERSION          : %d.%d.%d\n",
           PAPI_VERSION_MAJOR(PAPI_VERSION),
           PAPI_VERSION_MINOR(PAPI_VERSION),
           PAPI_VERSION_REVISION(PAPI_VERSION));

    /* 2. Try to set up Hardware Event Counters */
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

    /* 3. Benchmark using PAPI Software / OS Timers */
    rtime_start = PAPI_get_real_usec();
    vtime_start = PAPI_get_virt_usec();
    rcyc_start  = PAPI_get_real_cyc();
    vcyc_start  = PAPI_get_virt_cyc();

    do_work();

    rtime_end = PAPI_get_real_usec();
    vtime_end = PAPI_get_virt_usec();
    rcyc_end  = PAPI_get_real_cyc();
    vcyc_end  = PAPI_get_virt_cyc();

    /* 4. Stop and print HW counters (if active) */
    if (hw_counters_active) {
        retval = PAPI_stop(EventSet, values);
        if (retval == PAPI_OK) {
            printf("\n--- Hardware Counters ---\n");
            for (i = 0; i < NUM_EVENTS; i++) {
                printf("%12lld \t --> %s\n", values[i], EventName[i]);
            }
        }
    } else {
        printf("\nNOTICE: HW counters unavailable (kernel.perf_event_paranoid active).\n");
    }

    /* 5. Print Software / OS Timer Results (Always Available) */
    long long elapsed_rtime = rtime_end - rtime_start;
    long long elapsed_vtime = vtime_end - vtime_start;
    long long elapsed_rcyc  = rcyc_end - rcyc_start;
    long long elapsed_vcyc  = vcyc_end - vcyc_start;

    printf("\n--- PAPI Software/OS Timers ---\n");
    printf("Real Time (wall-clock) : %lld us\n", elapsed_rtime);
    printf("Virtual Time (CPU time): %lld us\n", elapsed_vtime);
    printf("Real Cycles            : %lld cycles\n", elapsed_rcyc);
    printf("Virtual Cycles         : %lld cycles\n", elapsed_vcyc);

    /* 6. Sanity check: Ensure software timers returned valid readings */
    if (elapsed_rtime <= 0 || elapsed_vtime <= 0) {
        printf("ERROR: PAPI software timers returned invalid measurements!\n");
        exit(EXIT_FAILURE);
    }

    PAPI_shutdown();
    printf("\nTEST PASSED (Software timer validation successful).\n");
    return EXIT_SUCCESS;
}
