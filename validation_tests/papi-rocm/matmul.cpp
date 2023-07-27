#include <stdio.h>
#include "matmul.h"

#define BLOCK_DIM_X (16)
#define BLOCK_DIM_Y (16)
#define ROWS        (4096)
#define COLS        (ROWS)

__global__ void matmul(float *A, float *B, float *C, int N)
{
    int i = (hipBlockIdx_y * hipBlockDim_y) + hipThreadIdx_y;
    int j = (hipBlockIdx_x * hipBlockDim_x) + hipThreadIdx_x;

    if (i < N && j < N) {
        float sum = 0.0;
        for (int k = 0; k < N; ++k) {
            sum += A[(i * N) + k] * B[(k * N) + j];
        }
        C[(i * N) + j] = sum;
    }
}

struct matmul_arrays {
    float *h_A;
    float *h_B;
    float *h_C;
    float *d_A;
    float *d_B;
    float *d_C;
};

int matmul_init(void **handle)
{
    hipError_t hip_errno;

    struct matmul_arrays *handle_p = (struct matmul_arrays *) malloc(sizeof(*handle_p));
    if (handle_p == NULL) {
        return MATMUL_ENOMEM;
    }

    hip_errno = hipHostMalloc(&handle_p->h_A, sizeof(float) * ROWS * COLS);
    if (hip_errno != hipSuccess) {
        return MATMUL_ENOMEM;
    }

    hip_errno = hipHostMalloc(&handle_p->h_B, sizeof(float) * ROWS * COLS);
    if (hip_errno != hipSuccess) {
        return MATMUL_ENOMEM;
    }

    hip_errno = hipHostMalloc(&handle_p->h_C, sizeof(float) * ROWS * COLS);
    if (hip_errno != hipSuccess) {
        return MATMUL_ENOMEM;
    }

    hip_errno = hipMalloc(&handle_p->d_A, sizeof(float) * ROWS * COLS);
    if (hip_errno != hipSuccess) {
        return MATMUL_ENOMEM;
    }

    hip_errno = hipMalloc(&handle_p->d_B, sizeof(float) * ROWS * COLS);
    if (hip_errno != hipSuccess) {
        return MATMUL_ENOMEM;
    }

    hip_errno = hipMalloc(&handle_p->d_C, sizeof(float) * ROWS * COLS);
    if (hip_errno != hipSuccess) {
        return MATMUL_ENOMEM;
    }

    for (int i = 0; i < ROWS * COLS; ++i) {
        handle_p->h_A[i] = handle_p->h_B[i] = (float) (rand() % 1000);
        handle_p->h_C[i] = 0.0;
    }

    *handle = handle_p;

    return MATMUL_SUCCESS;
}

int matmul_run(void *handle, hipStream_t stream)
{
    hipError_t hip_errno;
    float *h_A, *h_B, *h_C, *d_A, *d_B, *d_C;

    struct matmul_arrays *handle_p = (struct matmul_arrays *) handle;
    h_A = handle_p->h_A;
    h_B = handle_p->h_B;
    h_C = handle_p->h_C;
    d_A = handle_p->d_A;
    d_B = handle_p->d_B;
    d_C = handle_p->d_C;

    hip_errno = hipMemcpyAsync(d_A, h_A, sizeof(float) * ROWS * COLS, hipMemcpyHostToDevice, stream);
    if (hip_errno != hipSuccess) {
        return MATMUL_EMISC;
    }

    hip_errno = hipMemcpyAsync(d_B, h_B, sizeof(float) * ROWS * COLS, hipMemcpyHostToDevice, stream);
    if (hip_errno != hipSuccess) {
        return MATMUL_EMISC;
    }

    dim3 grid_dim  = dim3(ROWS / BLOCK_DIM_X, COLS / BLOCK_DIM_Y);
    dim3 block_dim = dim3(BLOCK_DIM_X, BLOCK_DIM_Y);

    hipLaunchKernelGGL(matmul, grid_dim, block_dim, 0, stream, d_A, d_B, d_C, ROWS);
    hip_errno = hipGetLastError();
    if (hip_errno != hipSuccess) {
        return MATMUL_EMISC;
    }

    hip_errno = hipMemcpyAsync(h_C, d_C, sizeof(float) * ROWS * COLS, hipMemcpyDeviceToHost, stream);
    if (hip_errno != hipSuccess) {
        return MATMUL_EMISC;
    }

    return MATMUL_SUCCESS;
}

int matmul_finalize(void **handle)
{
    struct matmul_arrays *handle_p = (struct matmul_arrays *) (*handle);
    hipFree(handle_p->h_A);
    hipFree(handle_p->h_B);
    hipFree(handle_p->h_C);
    hipFree(handle_p->d_A);
    hipFree(handle_p->d_B);
    hipFree(handle_p->d_C);
    free(handle_p);
    *handle = NULL;
    return MATMUL_SUCCESS;
}
