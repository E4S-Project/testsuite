int64_t hits = 0; 
// counts the number of ranks for which the RPC has completed
int n_done = 0;

int64_t reduce_to_rank0(int64_t my_hits)
{
    // cannot wait for the RPC - there is no return
    upcxx::rpc_ff(0, [](int64_t my_hits) { hits += my_hits; n_done++; }, my_hits);
    if (upcxx::rank_me() == 0) {
        // spin waiting for RPCs from all ranks to complete
        // When spinning, call the progress function to 
        // ensure rank 0 processes waiting RPCs
        while (n_done != upcxx::rank_n()) upcxx::progress();
    }
    // wait until all RPCs have been processed (quiescence)
    upcxx::barrier();
    return hits;
}
