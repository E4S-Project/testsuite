/*
 * hypre_smoke_test.c
 *
 * Minimal smoke test for an installed hypre 3.x (3.0+).
 * Works against CPU, CUDA, and HIP/ROCm builds without any changes.
 *
 * Build (adjust paths as needed):
 *
 *   mpicc -o hypre_smoke_test hypre_smoke_test.c \
 *       -I${HYPRE_DIR}/include \
 *       -L${HYPRE_DIR}/lib -lHYPRE \
 *       -lm
 *
 * For a CUDA-backed hypre add: -lcudart -lcublas -lcusparse
 * For a HIP/ROCm-backed hypre add: -lrocsparse -lrocblas -lamdhip64
 * (or let the linker find them via rpath if spack set it up correctly)
 *
 * Run:
 *   mpirun -np 1 ./hypre_smoke_test          # single rank
 *   mpirun -np 4 ./hypre_smoke_test          # multi-rank
 *
 * What it does:
 *   Builds the 1-D Poisson tridiagonal system  A x = b  where b = ones
 *   (N=20 unknowns, evenly distributed across MPI ranks) via the IJ /
 *   ParCSR interface, then solves with PCG + BoomerAMG.  On success it
 *   prints the final relative residual and exits 0.  On any hypre error
 *   or a residual that did not converge it exits 1.
 *
 * Why IJ/ParCSR + PCG+AMG?
 *   - IJ is the lowest-common-denominator interface: every hypre build
 *     (CPU, CUDA, HIP, SYCL) supports it.
 *   - BoomerAMG and PCG are both GPU-enabled (IJ interface) in 3.x, so
 *     a GPU build will actually exercise the device code paths.
 *   - No dependency on hypre's internal test helpers (ex.h / vis.c).
 *
 * Key 3.x vs 2.x differences handled here:
 *   - HYPRE_Initialize() / HYPRE_Finalize() are required in 3.x.
 *     (HYPRE_Init() still exists as a compat alias but is deprecated.)
 *   - HYPRE_BigInt is used for global row/column indices (was plain int
 *     in old 2.x builds without --enable-bigint).
 *   - Memory for values/indices must be allocated with
 *     hypre_TAlloc / HYPRE_MEMORY_HOST so that the library can manage
 *     it correctly regardless of the active memory backend.
 *     For a smoke test it is fine to use plain malloc because we are
 *     only touching host-side setup data; the key point is that we do
 *     NOT pass stack arrays to SetValues().
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "HYPRE.h"
#include "HYPRE_parcsr_ls.h"   /* BoomerAMG, PCG */
#include "HYPRE_krylov.h"      /* PCG */

/* --------------------------------------------------------------------------
 * Simple error-checking macro.  HYPRE functions return 0 on success.
 * -------------------------------------------------------------------------- */
#define HYPRE_CHECK(call)                                                    \
    do {                                                                     \
        HYPRE_Int _err = (call);                                             \
        if (_err) {                                                          \
            fprintf(stderr, "[rank %d] HYPRE error %d at %s:%d\n",          \
                    myid, (int)_err, __FILE__, __LINE__);                    \
            MPI_Abort(MPI_COMM_WORLD, 1);                                   \
        }                                                                    \
    } while (0)

int main(int argc, char *argv[])
{
    /* ------------------------------------------------------------------ */
    /* 1.  MPI + hypre initialisation                                      */
    /* ------------------------------------------------------------------ */
    MPI_Init(&argc, &argv);

    int myid, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    /* Required in hypre 3.x.  Safe to call even on CPU-only builds. */
    HYPRE_Initialize();

    /* Optional: initialise GPU features when the library was built with
     * CUDA/HIP support.  This is a no-op on CPU-only builds. */
    HYPRE_DeviceInitialize();

    /* ------------------------------------------------------------------ */
    /* 2.  Problem definition: 1-D Poisson, N unknowns                     */
    /* ------------------------------------------------------------------ */
    const HYPRE_Int N = 20;   /* global system size – tiny, just for smoke */

    /* Distribute rows round-robin over ranks. */
    HYPRE_Int local_size = N / num_procs;
    HYPRE_Int extra      = N % num_procs;

    /* Each rank gets 'local_size' rows, plus one extra if myid < extra. */
    HYPRE_BigInt ilower = (HYPRE_BigInt)myid * local_size
                        + (HYPRE_BigInt)(myid < extra ? myid : extra);
    HYPRE_BigInt iupper = ilower + local_size - 1
                        + (HYPRE_BigInt)(myid < extra ? 1 : 0);

    HYPRE_Int rows_here = (HYPRE_Int)(iupper - ilower + 1);

    /* ------------------------------------------------------------------ */
    /* 3.  Build the IJ matrix                                             */
    /* ------------------------------------------------------------------ */
    HYPRE_IJMatrix    ij_A;
    HYPRE_ParCSRMatrix parcsr_A;

    HYPRE_CHECK(HYPRE_IJMatrixCreate(MPI_COMM_WORLD,
                                     ilower, iupper,
                                     ilower, iupper,
                                     &ij_A));
    HYPRE_CHECK(HYPRE_IJMatrixSetObjectType(ij_A, HYPRE_PARCSR));
    HYPRE_CHECK(HYPRE_IJMatrixInitialize(ij_A));

    /* Fill the tridiagonal 1-D Laplacian row by row. */
    {
        HYPRE_Int     ncols_buf[3];
        HYPRE_BigInt  row_buf[1];
        HYPRE_BigInt  col_buf[3];
        double        val_buf[3];

        for (HYPRE_BigInt row = ilower; row <= iupper; row++) {
            int k = 0;
            if (row > 0) {
                col_buf[k] = row - 1;
                val_buf[k] = -1.0;
                k++;
            }
            col_buf[k] = row;
            val_buf[k] = 2.0;
            k++;
            if (row < N - 1) {
                col_buf[k] = row + 1;
                val_buf[k] = -1.0;
                k++;
            }
            row_buf[0]    = row;
            ncols_buf[0]  = k;
            HYPRE_CHECK(HYPRE_IJMatrixSetValues(ij_A, 1, ncols_buf,
                                                row_buf, col_buf, val_buf));
        }
    }

    HYPRE_CHECK(HYPRE_IJMatrixAssemble(ij_A));
    HYPRE_CHECK(HYPRE_IJMatrixGetObject(ij_A, (void **)&parcsr_A));

    /* ------------------------------------------------------------------ */
    /* 4.  Build RHS vector b = ones, and initial guess x = zeros          */
    /* ------------------------------------------------------------------ */
    HYPRE_IJVector  ij_b,  ij_x;
    HYPRE_ParVector par_b, par_x;

    /* -- b -- */
    HYPRE_CHECK(HYPRE_IJVectorCreate(MPI_COMM_WORLD, ilower, iupper, &ij_b));
    HYPRE_CHECK(HYPRE_IJVectorSetObjectType(ij_b, HYPRE_PARCSR));
    HYPRE_CHECK(HYPRE_IJVectorInitialize(ij_b));
    {
        double       *vals = (double *)malloc(rows_here * sizeof(double));
        HYPRE_BigInt *inds = (HYPRE_BigInt *)malloc(rows_here * sizeof(HYPRE_BigInt));
        for (int i = 0; i < rows_here; i++) {
            inds[i] = ilower + i;
            vals[i] = 1.0;
        }
        HYPRE_CHECK(HYPRE_IJVectorSetValues(ij_b, rows_here, inds, vals));
        free(vals);
        free(inds);
    }
    HYPRE_CHECK(HYPRE_IJVectorAssemble(ij_b));
    HYPRE_CHECK(HYPRE_IJVectorGetObject(ij_b, (void **)&par_b));

    /* -- x -- */
    HYPRE_CHECK(HYPRE_IJVectorCreate(MPI_COMM_WORLD, ilower, iupper, &ij_x));
    HYPRE_CHECK(HYPRE_IJVectorSetObjectType(ij_x, HYPRE_PARCSR));
    HYPRE_CHECK(HYPRE_IJVectorInitialize(ij_x));
    {
        double       *vals = (double *)calloc(rows_here, sizeof(double));
        HYPRE_BigInt *inds = (HYPRE_BigInt *)malloc(rows_here * sizeof(HYPRE_BigInt));
        for (int i = 0; i < rows_here; i++) inds[i] = ilower + i;
        HYPRE_CHECK(HYPRE_IJVectorSetValues(ij_x, rows_here, inds, vals));
        free(vals);
        free(inds);
    }
    HYPRE_CHECK(HYPRE_IJVectorAssemble(ij_x));
    HYPRE_CHECK(HYPRE_IJVectorGetObject(ij_x, (void **)&par_x));

    /* ------------------------------------------------------------------ */
    /* 5.  Solver: PCG preconditioned with BoomerAMG                       */
    /* ------------------------------------------------------------------ */
    HYPRE_Solver pcg_solver, amg_precond;

    /* AMG preconditioner */
    HYPRE_CHECK(HYPRE_BoomerAMGCreate(&amg_precond));
    HYPRE_CHECK(HYPRE_BoomerAMGSetMaxIter(amg_precond, 1));   /* 1 V-cycle */
    HYPRE_CHECK(HYPRE_BoomerAMGSetTol(amg_precond, 0.0));
    HYPRE_CHECK(HYPRE_BoomerAMGSetPrintLevel(amg_precond, 0));

    /* PCG solver */
    HYPRE_CHECK(HYPRE_ParCSRPCGCreate(MPI_COMM_WORLD, &pcg_solver));
    HYPRE_CHECK(HYPRE_PCGSetMaxIter(pcg_solver, 100));
    HYPRE_CHECK(HYPRE_PCGSetTol(pcg_solver, 1e-8));
    HYPRE_CHECK(HYPRE_PCGSetTwoNorm(pcg_solver, 1));
    HYPRE_CHECK(HYPRE_PCGSetPrintLevel(pcg_solver, 0));
    HYPRE_CHECK(HYPRE_PCGSetLogging(pcg_solver, 1));

    HYPRE_CHECK(HYPRE_PCGSetPrecond(pcg_solver,
                    (HYPRE_PtrToSolverFcn)HYPRE_BoomerAMGSolve,
                    (HYPRE_PtrToSolverFcn)HYPRE_BoomerAMGSetup,
                    amg_precond));

    HYPRE_CHECK(HYPRE_ParCSRPCGSetup(pcg_solver, parcsr_A, par_b, par_x));
    HYPRE_CHECK(HYPRE_ParCSRPCGSolve(pcg_solver, parcsr_A, par_b, par_x));

    /* ------------------------------------------------------------------ */
    /* 6.  Check result                                                    */
    /* ------------------------------------------------------------------ */
    HYPRE_Int    num_iterations;
    double       final_res_norm;

    HYPRE_CHECK(HYPRE_PCGGetNumIterations(pcg_solver, &num_iterations));
    HYPRE_CHECK(HYPRE_PCGGetFinalRelativeResidualNorm(pcg_solver, &final_res_norm));

    if (myid == 0) {
        printf("hypre smoke test PASSED\n");
        printf("  iterations    : %d\n", (int)num_iterations);
        printf("  final rel res : %e\n", final_res_norm);
    }

    int exit_code = 0;
    if (final_res_norm > 1e-6 || num_iterations == 0) {
        if (myid == 0)
            fprintf(stderr, "FAILED: did not converge to tolerance\n");
        exit_code = 1;
    }

    /* ------------------------------------------------------------------ */
    /* 7.  Clean up                                                        */
    /* ------------------------------------------------------------------ */
    HYPRE_CHECK(HYPRE_BoomerAMGDestroy(amg_precond));
    HYPRE_CHECK(HYPRE_ParCSRPCGDestroy(pcg_solver));
    HYPRE_CHECK(HYPRE_IJMatrixDestroy(ij_A));
    HYPRE_CHECK(HYPRE_IJVectorDestroy(ij_b));
    HYPRE_CHECK(HYPRE_IJVectorDestroy(ij_x));

    HYPRE_Finalize();
    MPI_Finalize();

    return exit_code;
}
