int64_t hits = 0;
upcxx::promise<> prom;
upcxx::global_ptr<int64_t> all_hits_ptr = nullptr;

int64_t reduce_to_rank0(int64_t my_hits)
{
    if (upcxx::rank_me() == 0) 
        prom.require_anonymous(upcxx::rank_n());
    if (upcxx::rank_me() == 0) 
        all_hits_ptr = upcxx::new_array<int64_t>(upcxx::rank_n());
    // Rank 0 broadcasts the array global pointer to all ranks
    all_hits_ptr = upcxx::broadcast(all_hits_ptr, 0).wait();
    // All ranks offset the start pointer of the array by their rank to point
    // to their own chunk of the array
    upcxx::global_ptr<int64_t> my_hits_ptr = all_hits_ptr + upcxx::rank_me();
    // operation completions
    auto cxs = (upcxx::operation_cx::as_future() |
                upcxx::remote_cx::as_rpc(
                    [](upcxx::intrank_t rank) {
                        hits += *(all_hits_ptr + rank).local();
                        prom.fulfill_anonymous(1);
                    }, upcxx::rank_me()));
    // all ranks try to write to their own part on rank 0 and then accumulate
    auto result = upcxx::rput(my_hits, my_hits_ptr, cxs);
    result.wait();
    upcxx::future<> fut = prom.finalize();
    fut.wait();
    if (upcxx::rank_me() == 0)
        upcxx::delete_array(all_hits_ptr);
    return hits;
}
