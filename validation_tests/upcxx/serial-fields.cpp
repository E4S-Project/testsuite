#include <upcxx/upcxx.hpp>

#define N 32

//SNIPPET
class dist_reduction {
    public:
        // The values to perform a sum reduction across
        double values[N];
        // Used to store a local sum reduction result on each rank
        double partial_sum_reduction;

        // Default constructor used by UPC++ serialization
        dist_reduction() {
            for (int i = 0; i < N; i++) { values[i] = 1.; }
        }

        void calculate_partial_sum_reduction() {
            partial_sum_reduction = 0.0;
            for (int i = 0; i < N; i++) { partial_sum_reduction += values[i]; }
        }

        UPCXX_SERIALIZED_FIELDS(partial_sum_reduction)
};
//SNIPPET

int main(void) {
    upcxx::init();

    int rank = upcxx::rank_me();
    int nranks = upcxx::rank_n();

    upcxx::dist_object<double> sum_reduction(0);

    // Compute a local sum reduction
    dist_reduction reduce;
    reduce.calculate_partial_sum_reduction();

    /*
     * Compute a global sum reduction by sending local results to rank 0. Note
     * that this captures an object of type dist_reduction, but thanks to UPC++
     * serialization we only need to send one double over the network.
     */
    upcxx::rpc(0, [] (upcxx::dist_object<double>& sum_reduction,
                    const dist_reduction& reduce) {
                *sum_reduction += reduce.partial_sum_reduction;
            }, sum_reduction, reduce).wait();

    upcxx::barrier();

    if (rank == 0) {
        assert(*sum_reduction == nranks * N);
        std::cout << "Rank 0 out of " << nranks << " got a sum of " <<
            *sum_reduction << std::endl;
        std::cout << "SUCCESS" << std::endl;
    }

    upcxx::finalize();

    return 0;
}
