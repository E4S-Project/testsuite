#include <stdio.h>
#include <string.h>

#define FAILURE -1
#define SUCCESS  0

#define TYPED_ALLOC(type, name, size) \
	type name = (type) malloc(size); \
	if (name == NULL) { \
          fprintf(stderr, "failed to allocate host vector " #name ".\n"); \
          exit(FAILURE); \
        }

#define CUDA_CALL(call, format) \
	{ cudaError_t err = call; \
	  if (err != cudaSuccess) { \
            fprintf(stderr, format " error: %s\n",  cudaGetErrorString(err)); \
            exit(FAILURE); \
          } \
        }

__global__ void
vecadd_kernel(int *C, const int *A, const int *B, int n) {
    int i = blockDim.x * blockIdx.x + threadIdx.x;
    if (i < n) C[i] = A[i] + B[i];
}


int vecadd(int device) {

    if (device > 0) CUDA_CALL(cudaSetDevice(device), "failed to set CUDA device");

    int n = 10000;
    size_t size = n * sizeof(int);

    printf("add two integer vectors of %d elements on a GPU\n", n);

    // allocate host vectors
    TYPED_ALLOC(int *, h_A, size);
    TYPED_ALLOC(int *, h_B, size);
    TYPED_ALLOC(int *, h_C, size);

    // initialize host input vectors h_A and h_B
    memset(h_A, 1, size); 
    memset(h_B, 2, size); 

    // allocate GPU vectors
    int *d_A = NULL;
    int *d_B = NULL;
    int *d_C = NULL;
    CUDA_CALL(cudaMalloc((void **)&d_A, size), "failed to allocate GPU vector d_A.");
    CUDA_CALL(cudaMalloc((void **)&d_B, size), "failed to allocate GPU vector d_B.");
    CUDA_CALL(cudaMalloc((void **)&d_C, size), "failed to allocate GPU vector d_C.");

    printf("copy h_A and h_B from host to GPU\n");
    CUDA_CALL(cudaMemcpy(d_A, h_A, size, cudaMemcpyHostToDevice), "failed to copy vector h_A from host to GPU.");
    CUDA_CALL(cudaMemcpy(d_B, h_B, size, cudaMemcpyHostToDevice), "failed to copy vector h_B from host to GPU.");

    // launch the vector add kernel
    int threads = 256;
    int blocks = (n + threads - 1) / threads;
    printf("launch vecadd_kernel with %d blocks of %d threads\n", blocks, threads);
    vecadd_kernel<<<blocks, threads>>>(d_C, d_A, d_B, n);

    CUDA_CALL(cudaGetLastError(), "failed to launch vecadd_kernel.");

    // copy the result from GPU to host 
    printf("copy d_C from GPU to the host\n");
    CUDA_CALL(cudaMemcpy(h_C, d_C, size, cudaMemcpyDeviceToHost), "failed to copy vector d_C from GPU to host.");

    // verify the result
    for (int i = 0; i < n; ++i) {
        if (h_A[i] + h_B[i] != h_C[i]) {
            fprintf(stderr, "bad sum: h_C[%d]=%d\n", i, h_C[i]);
            exit(FAILURE);
        }
    }

    // free vectors on host
    free(h_A);
    free(h_B);
    free(h_C);

    // free vectors on GPU
    CUDA_CALL(cudaFree(d_A), "failed to free GPU vector d_A.");
    CUDA_CALL(cudaFree(d_B), "failed to free GPU vector d_B.");
    CUDA_CALL(cudaFree(d_C), "failed to free GPU vector d_C.");

    return 0;
}

