#include <AMReX.H>
#include <AMReX_Print.H>
#include <AMReX_Gpu.H>
#include <AMReX_MultiFab.H>

int main(int argc, char* argv[]) {
    amrex::Initialize(argc, argv);
    {
        amrex::Print() << "AMReX test initialized successfully across " 
                       << amrex::ParallelDescriptor::NProcs() << " ranks!\n";

        // Create 16x16x16 grid box (4096 total cells)
        amrex::Box box(amrex::IntVect(0,0,0), amrex::IntVect(15,15,15));
        amrex::BoxArray ba(box);
        
        // Break the box into 8 sub-grids (8x8x8 each) to distribute across ranks
        ba.maxSize(8);

        amrex::DistributionMapping dm(ba);

        // Allocate memory across ranks
        amrex::MultiFab mf(ba, dm, 1, 0);
        mf.setVal(0.0);

        // Execute parallel kernel across all assigned ranks
        for (amrex::MFIter mfi(mf); mfi.isValid(); ++mfi) {
            const amrex::Box& bx = mfi.validbox();
            auto const& arr = mf.array(mfi);
            amrex::ParallelFor(bx, [=] AMREX_GPU_DEVICE (int i, int j, int k) noexcept {
                arr(i,j,k) = 1.0;
            });
        }
        amrex::Gpu::synchronize();

        // Reduce sum across all ranks
        double total = mf.sum(0);
        amrex::Print() << "MultiFab sum: " << total << " (Expected: 4096.0)\n";

        if (total != 4096.0) {
            amrex::Abort("MultiFab sum check failed!");
        }
    }
    amrex::Finalize();
    return 0;
}
