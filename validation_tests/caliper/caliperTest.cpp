#include <caliper/cali.h>

int main(int argc, char* argv[])
{
    // Mark this function
    CALI_CXX_MARK_FUNCTION;

    // Mark the "intialization" phase
    CALI_MARK_BEGIN("initialization");
    int count = 4;
    double t = 0.0, delta_t = 1e-6;
    CALI_MARK_END("initialization");

    // Mark the loop
    CALI_CXX_MARK_LOOP_BEGIN(mainloop, "main loop");

    for (int i = 0; i < count; ++i) {
        // Mark each loop iteration
        CALI_CXX_MARK_LOOP_ITERATION(mainloop, i);

        // A Caliper snapshot taken at this point will contain
        // { "function"="main", "loop"="main loop", "iteration#main loop"=<i> }

        // ...
    }

    CALI_CXX_MARK_LOOP_END(mainloop);
}
