/****************************/
/* THIS IS OPEN SOURCE CODE */
/****************************/

/**
 * @file    HelloWorld.cu
 * @author  Heike Jagode
 *          jagode@eecs.utk.edu
 * Mods:	Anustuv Pal
 *			anustuv@icl.utk.edu
 * Mods:	<your name here>
 *			<your email address>
 * test case for Example component
 *
 *
 * @brief
 *  This file is a very simple HelloWorld C example which serves (together
 *	with its Makefile) as a guideline on how to add tests to components.
 *  The papi configure and papi Makefile will take care of the compilation
 *	of the component tests (if all tests are added to a directory named
 *	'tests' in the specific component dir).
 *	See components/README for more details.
 *
 *	The string "Hello World!" is mangled and then restored.
 *
 *  CUDA Context notes for CUPTI_11: Although a cudaSetDevice() will create a
 *  primary context for the device that allows kernel execution; PAPI cannot
 *  use a primary context to control the Nvidia Performance Profiler.
 *  Applications must create a context using cuCtxCreate() that will execute
 *  the kernel, this must be done prior to the PAPI_add_events() invocation in
 *  the code below. If multiple GPUs are in use, each requires its own context,
 *  and that context should be active when PAPI_events are added for each
 *  device.  Which means using Seperate PAPI_add_events() for each device. For
 *  an example see simpleMultiGPU.cu.
 *
 *  There are three points below where cuCtxCreate() is called, this code works
 *  if any one of them is used alone.
 */

#include <cuda.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef PAPI
#include "papi.h"
#endif

#define STEP_BY_STEP_DEBUG 0 /* helps debug CUcontext issues. */

// Prototypes
__global__ void helloWorld(char*);


// Host function
int main(int argc, char** argv)
{
    CUcontext getCtx=NULL, sessionCtx=NULL;
    cudaError_t cudaError;
    CUresult cuError; (void) cuError;
    int retval = 0;
#ifdef PAPI

    /* PAPI Initialization */
    int papi_errno = PAPI_library_init( PAPI_VER_CURRENT );
    if( papi_errno != PAPI_VER_CURRENT ) {
        fprintf(stderr, "PAPI_library_init failed.\n");
        retval++;
    }

    printf( "PAPI_VERSION     : %4d %6d %7d\n",
        PAPI_VERSION_MAJOR( PAPI_VERSION ),
        PAPI_VERSION_MINOR( PAPI_VERSION ),
        PAPI_VERSION_REVISION( PAPI_VERSION ) );

    int i;
    int EventSet = PAPI_NULL;
    int eventCount = argc - 1;

    /* if no events passed at command line, just report test skipped. */
    if (eventCount == 0) {
        fprintf(stderr, "No eventnames specified at command line.\n");
        retval++;
    }

    long long *values = (long long *) calloc(eventCount, sizeof (long long));
    if (values == NULL) {
        fprintf(stderr, "Failed to allocate memory for values.\n");
        retval++;
    }
    int *events = (int *) calloc(eventCount, sizeof (int));
    if (events == NULL) {
        fprintf(stderr, "Failed to allocate memory for events.\n");
        retval++;
    }
    /* convert PAPI native events to PAPI code */
    for( i = 0; i < eventCount; i++ ){
        papi_errno = PAPI_event_name_to_code( argv[i+1], &events[i] );
        if( papi_errno != PAPI_OK ) {
            fprintf(stderr, "Check event name: %s", argv[i+1] );
        }
        printf("Name %s --- Code: %#x\n", argv[i+1], events[i]);
    }

    if (STEP_BY_STEP_DEBUG) {
        cuCtxGetCurrent(&getCtx);
        fprintf(stderr, "%s:%s:%i before PAPI_create_eventset() getCtx=%p.\n", __FILE__, __func__, __LINE__, getCtx);
    }

    papi_errno = PAPI_create_eventset( &EventSet );
    if( papi_errno != PAPI_OK ) {
        fprintf(stderr, "Cannot create eventset.\n");
        retval++;
    }

    if (STEP_BY_STEP_DEBUG) {
        cuCtxGetCurrent(&getCtx);
        fprintf(stderr, "%s:%s:%i before PAPI_add_events(), getCtx=%p.\n", __FILE__, __func__, __LINE__, getCtx);
    }

    // If multiple GPUs/contexts were being used, you'd need to
    // create contexts for each device. See, for example,
    // simpleMultiGPU.cu.

    // Context Create. We will use this one to run our kernel.
    cuError = cuCtxCreate(&sessionCtx, 0, 0); // Create a context, NULL flags, Device 0.
    if (cuError != CUDA_SUCCESS) {
        fprintf(stderr, "Failed to create cuContext.\n");
        retval++;
    }

    if (STEP_BY_STEP_DEBUG) {
        cuCtxGetCurrent(&getCtx);
        fprintf(stderr, "%s:%s:%i after cuCtxCreate(&sessionCtx), about to PAPI_start(), sessionCtx=%p, getCtx=%p.\n", __FILE__, __func__, __LINE__, sessionCtx, getCtx);
    }

    papi_errno = PAPI_add_events( EventSet, events, eventCount );
    if (papi_errno == PAPI_ENOEVNT) {
        fprintf(stderr, "Event name does not exist for component.");
    }
    if( papi_errno != PAPI_OK ) {
        fprintf(stderr, "PAPI_add_events failed.\n");
        retval++;
    }

    if (STEP_BY_STEP_DEBUG) {
        cuCtxGetCurrent(&getCtx);
        fprintf(stderr, "%s:%s:%i before PAPI_start(), getCtx=%p.\n", __FILE__, __func__, __LINE__, getCtx);
    }

    papi_errno = PAPI_start( EventSet );
    if( papi_errno != PAPI_OK ) {
        fprintf(stderr, "PAPI_start failed.\n");
        retval++;
    }

    if (STEP_BY_STEP_DEBUG) {
        cuCtxGetCurrent(&getCtx);
        fprintf(stderr, "%s:%s:%i after PAPI_start(), getCtx=%p.\n", __FILE__, __func__, __LINE__, getCtx);
    }

#endif

    int j;

    // desired output
    char str[] = "Hello World!";

    // mangle contents of output
    // the null character is left intact for simplicity
    for(j = 0; j < 12; j++) {
        str[j] -= j;
    }

    printf("mangled str=%s\n", str);

    // allocate memory on the device
    char *d_str;
    size_t size = sizeof(str);
    cudaMalloc((void**)&d_str, size);

    if (STEP_BY_STEP_DEBUG) {
        cuCtxGetCurrent(&getCtx);
        fprintf(stderr, "%s:%s:%i after cudaMalloc() getCtx=%p.\n", __FILE__, __func__, __LINE__, getCtx);
    }

    // copy the string to the device
    cudaMemcpy(d_str, str, size, cudaMemcpyHostToDevice);

    if (STEP_BY_STEP_DEBUG) {
        cuCtxGetCurrent(&getCtx);
        fprintf(stderr, "%s:%s:%i after cudaMemcpy(ToDevice) getCtx=%p.\n", __FILE__, __func__, __LINE__, getCtx);
    }

    // set the grid and block sizes
    dim3 dimGrid(2); // one block per word
    dim3 dimBlock(6); // one thread per character

    // invoke the kernel
    helloWorld<<< dimGrid, dimBlock >>>(d_str);

    cudaError = cudaGetLastError();
    if (STEP_BY_STEP_DEBUG) {
        fprintf(stderr, "%s:%s:%i Kernel Return Code: %s.\n", __FILE__, __func__, __LINE__, cudaGetErrorString(cudaError));
    }

    if (STEP_BY_STEP_DEBUG) {
        cuCtxGetCurrent(&getCtx);
        fprintf(stderr, "%s:%s:%i After Kernel Execution: getCtx=%p.\n", __FILE__, __func__, __LINE__, getCtx);
    }

    // retrieve the results from the device
    cudaMemcpy(str, d_str, size, cudaMemcpyDeviceToHost);

    if (STEP_BY_STEP_DEBUG) {
        cuCtxGetCurrent(&getCtx);
        fprintf(stderr, "%s:%s:%i after cudaMemcpy(ToHost) getCtx=%p.\n", __FILE__, __func__, __LINE__, getCtx);
    }

    // free up the allocated memory on the device
    cudaFree(d_str);

    if (STEP_BY_STEP_DEBUG) {
        cuCtxGetCurrent(&getCtx);
        fprintf(stderr, "%s:%s:%i after cudaFree() getCtx=%p.\n", __FILE__, __func__, __LINE__, getCtx);
    }


#ifdef PAPI
    papi_errno = PAPI_read( EventSet, values );
    if( papi_errno != PAPI_OK ) {
        fprintf(stderr, "PAPI_read failed.\n");
        retval++;
    }

    if (STEP_BY_STEP_DEBUG) {
        cuCtxGetCurrent(&getCtx);
        fprintf(stderr, "%s:%s:%i after PAPI_read getCtx=%p.\n", __FILE__, __func__, __LINE__, getCtx);
    }

    for( i = 0; i < eventCount; i++ ) {
        printf("read: %12lld \t=0X%016llX \t\t --> %s \n", values[i], values[i], argv[i+1]);
    }

    papi_errno = cuCtxPopCurrent(&getCtx);
    if( papi_errno != CUDA_SUCCESS) {
        fprintf( stderr, "cuCtxPopCurrent failed, papi_errno=%d (%s)\n", papi_errno, PAPI_strerror(papi_errno) );
        retval++;
    }

    if (STEP_BY_STEP_DEBUG) {
        cuCtxGetCurrent(&getCtx);
        fprintf(stderr, "%s:%s:%i after cuCtxPopCurrent() getCtx=%p.\n", __FILE__, __func__, __LINE__, getCtx);
    }

    papi_errno = PAPI_stop( EventSet, values );
    if( papi_errno != PAPI_OK ) {
        fprintf(stderr, "PAPI_stop failed.\n");
        retval++;
    }

    if (STEP_BY_STEP_DEBUG) {
        cuCtxGetCurrent(&getCtx);
        fprintf(stderr, "%s:%s:%i after PAPI_stop getCtx=%p.\n", __FILE__, __func__, __LINE__, getCtx);
    }

    papi_errno = PAPI_cleanup_eventset(EventSet);
    if( papi_errno != PAPI_OK ) {
        fprintf(stderr, "PAPI_cleanup_eventset failed.\n");
        retval++;
    }

    if (STEP_BY_STEP_DEBUG) {
        cuCtxGetCurrent(&getCtx);
        fprintf(stderr, "%s:%s:%i after PAPI_cleanup_eventset getCtx=%p.\n", __FILE__, __func__, __LINE__, getCtx);
    }

    papi_errno = PAPI_destroy_eventset(&EventSet);
    if (papi_errno != PAPI_OK) {
        fprintf(stderr, "PAPI_destroy_eventset failed.\n");
        retval++;
    }

    if (STEP_BY_STEP_DEBUG) {
        cuCtxGetCurrent(&getCtx);
        fprintf(stderr, "%s:%s:%i after PAPI_destroy_eventset getCtx=%p.\n", __FILE__, __func__, __LINE__, getCtx);
    }


    for( i = 0; i < eventCount; i++ ) {
        printf("stop: %12lld \t=0X%016llX \t\t --> %s \n", values[i], values[i], argv[i+1]);
    }
#endif

    if (STEP_BY_STEP_DEBUG) {
        fprintf(stderr, "%s:%s:%i before cuCtxDestroy sessionCtx=%p.\n", __FILE__, __func__, __LINE__, sessionCtx);
    }

    // Test destroying the session Context.
    if (sessionCtx != NULL) {
        cuCtxDestroy(sessionCtx);
    }

    if (STEP_BY_STEP_DEBUG) {
        cuCtxGetCurrent(&getCtx);
        fprintf(stderr, "%s:%s:%i after cuCtxDestroy(%p) getCtx=%p.\n", __FILE__, __func__, __LINE__, sessionCtx, getCtx);
    }

#ifdef PAPI
    PAPI_shutdown();

    if (STEP_BY_STEP_DEBUG) {
        cuCtxGetCurrent(&getCtx);
        fprintf(stderr, "%s:%s:%i after PAPI_shutdown getCtx=%p.\n", __FILE__, __func__, __LINE__, getCtx);
    }

#endif
    return retval;
}


// Device kernel
__global__ void
helloWorld(char* str)
{
    // determine where in the thread grid we are
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    // unmangle output
    str[idx] += idx;
}
