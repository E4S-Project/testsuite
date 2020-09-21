int64_t reduce_to_rank0(int64_t my_hits)
{
    // initialize this rank's part of the distributed object with the local value
    upcxx::dist_object<int64_t> all_hits(my_hits);
    int64_t hits = 0;
    // rank 0 gets all the values asynchronously
    if (upcxx::rank_me() == 0) {
        hits = my_hits;
        upcxx::future<> f = upcxx::make_future();
        for (int i = 1; i < upcxx::rank_n(); i++) {
          // construct the conjoined futures
          f = upcxx::when_all(f,
            all_hits.fetch(i).then([&](int64_t rhit) { hits += rhit; })
          );
        }
        // wait for the futures to complete
        f.wait();
    }
    upcxx::barrier();
    return hits;
}
