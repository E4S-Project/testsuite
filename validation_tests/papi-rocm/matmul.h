#ifndef __MATMUL_H__
#define __MATMUL_H__

#include <hip/hip_runtime.h>

#define MATMUL_SUCCESS ( 0)
#define MATMUL_ENOMEM  (-1)
#define MATMUL_EMISC   (-2)

int matmul_init(void **handle);
int matmul_run(void *handle, hipStream_t stream);
int matmul_finalize(void **handle);

#endif /* End of __MATMUL_H__ */
