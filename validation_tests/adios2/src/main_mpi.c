/*
 * Adapted from ADIOS2 upstream testing/install/C/main_mpi.c (Apache-2.0):
 * writes and reads back a per-rank slice of a global array to exercise real
 * parallel I/O, not just linking.
 */

#include <adios2_c.h>

#if !ADIOS2_USE_MPI
#error "ADIOS2_USE_MPI is not true for source using ADIOS2 MPI bindings"
#endif

#include <mpi.h>

#include <stdint.h>
#include <stdio.h>

#define NX 10

int main(int argc, char **argv)
{
    int provided;

    /* MPI_THREAD_MULTIPLE is only required if you enable the SST MPI_DP */
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);

    int rank, nranks;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nranks);

    char bp_name[256];
    snprintf(bp_name, sizeof(bp_name), "%s.bp", argc > 0 ? argv[0] : "adios2_c_mpi_smoke_test");

    int32_t write_data[NX];
    for (int i = 0; i < NX; ++i)
    {
        write_data[i] = rank * 1000 + i;
    }

    adios2_adios *adios = adios2_init_mpi(MPI_COMM_WORLD);
    if (!adios)
    {
        fprintf(stderr, "adios2_init_mpi() failed\n");
        return 1;
    }

    adios2_io *io = adios2_declare_io(adios, "SmokeTestWriter");
    const size_t shape[1] = {(size_t)(NX * nranks)};
    const size_t start[1] = {(size_t)(NX * rank)};
    const size_t count[1] = {NX};
    adios2_variable *var = adios2_define_variable(io, "data", adios2_type_int32_t, 1, shape, start, count,
                                                   adios2_constant_dims_true);

    adios2_step_status status;
    adios2_engine *writer = adios2_open(io, bp_name, adios2_mode_write);
    adios2_begin_step(writer, adios2_step_mode_append, -1.0f, &status);
    adios2_put(writer, var, write_data, adios2_mode_sync);
    adios2_end_step(writer);
    adios2_close(writer);

    int32_t read_data[NX] = {0};
    adios2_io *rio = adios2_declare_io(adios, "SmokeTestReader");
    adios2_engine *reader = adios2_open(rio, bp_name, adios2_mode_read);
    adios2_begin_step(reader, adios2_step_mode_read, -1.0f, &status);
    adios2_variable *rvar = adios2_inquire_variable(rio, "data");
    if (!rvar)
    {
        fprintf(stderr, "[rank %d] variable 'data' not found on read\n", rank);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    adios2_set_selection(rvar, 1, start, count);
    adios2_get(reader, rvar, read_data, adios2_mode_sync);
    adios2_end_step(reader);
    adios2_close(reader);

    adios2_finalize(adios);

    int local_ok = 1;
    for (int i = 0; i < NX; ++i)
    {
        if (read_data[i] != write_data[i])
        {
            fprintf(stderr, "[rank %d] mismatch at %d: wrote %d read %d\n", rank, i, write_data[i], read_data[i]);
            local_ok = 0;
        }
    }

    int global_ok;
    MPI_Allreduce(&local_ok, &global_ok, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);

    if (rank == 0)
    {
        printf("adios2 C MPI smoke test: wrote/verified %d values across %d ranks: %s\n", NX * nranks, nranks,
               global_ok ? "PASS" : "FAIL");
    }

    MPI_Finalize();

    return global_ok ? 0 : 1;
}

