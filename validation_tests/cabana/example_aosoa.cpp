/****************************************************************************
 * Cabana AoSoA smoke test that works on host and device backends
 * (Serial, OpenMP, CUDA, HIP, SYCL...) via the Kokkos default execution space.
 ****************************************************************************/

#include <Cabana_Core.hpp>

#include <iostream>

//---------------------------------------------------------------------------//
// AoSoA example.
//---------------------------------------------------------------------------//
bool aosoaExample()
{
    std::cout << "Cabana AoSoA Example\n" << std::endl;

    using DataTypes = Cabana::MemberTypes<double[3][3], float[4], int>;
    const int VectorLength = 4;

    // Device (or host) memory space of the default backend.
    using ExecutionSpace = Kokkos::DefaultExecutionSpace;
    using MemorySpace = ExecutionSpace::memory_space;

    int num_tuple = 5;
    Cabana::AoSoA<DataTypes, MemorySpace, VectorLength> aosoa( "my_aosoa",
                                                               num_tuple );

    // Metadata calls are host-side and safe on any backend.
    std::cout << "aosoa.label() = " << aosoa.label() << std::endl;
    std::cout << "aosoa.size() = " << aosoa.size() << std::endl;
    std::cout << "aosoa.capacity() = " << aosoa.capacity() << std::endl;
    std::cout << "aosoa.numSoA() = " << aosoa.numSoA() << std::endl;

    // Host-accessible mirror. With a host-only backend this aliases the
    // original AoSoA (no allocation or copy); with a GPU backend it is a
    // separate host allocation.
    auto host_aosoa = Cabana::create_mirror_view( Kokkos::HostSpace(), aosoa );

    // ---- Host-side access: fill via 2D (SoA, lane) indexing ----
    for ( std::size_t s = 0; s < host_aosoa.numSoA(); ++s )
    {
        auto& soa = host_aosoa.access( s );

        for ( int i = 0; i < 3; ++i )
            for ( int j = 0; j < 3; ++j )
                for ( std::size_t a = 0; a < host_aosoa.arraySize( s ); ++a )
                    Cabana::get<0>( soa, a, i, j ) = 1.0 * ( a + i + j );

        for ( int i = 0; i < 4; ++i )
            for ( std::size_t a = 0; a < host_aosoa.arraySize( s ); ++a )
                Cabana::get<1>( soa, a, i ) = 1.0 * ( a + i );

        for ( std::size_t a = 0; a < host_aosoa.arraySize( s ); ++a )
            Cabana::get<2>( soa, a ) = a + 1234;
    }

    // ---- Host-side tuple set ----
    Cabana::Tuple<DataTypes> foo;
    for ( int i = 0; i < 3; ++i )
        for ( int j = 0; j < 3; ++j )
            Cabana::get<0>( foo, i, j ) = 1.1;
    for ( int i = 0; i < 4; ++i )
        Cabana::get<1>( foo, i ) = 2.2;
    Cabana::get<2>( foo ) = 3;
    host_aosoa.setTuple( 3, foo );

    // ---- Copy host data to the default memory space ----
    Cabana::deep_copy( aosoa, host_aosoa );

    // ---- Device kernel: add 1000 to the int member of every tuple ----
    {
        auto ints = Cabana::slice<2>( aosoa );
        Kokkos::parallel_for(
            "add_1000", Kokkos::RangePolicy<ExecutionSpace>( 0, num_tuple ),
            KOKKOS_LAMBDA( const int t ) { ints( t ) += 1000; } );
        Kokkos::fence();
    }

    // ---- Bring results back to the host and check them ----
    auto result =
        Cabana::create_mirror_view_and_copy( Kokkos::HostSpace(), aosoa );

    bool ok = true;
    for ( int t = 0; t < num_tuple; ++t )
    {
        auto tp = result.getTuple( t );

        for ( int i = 0; i < 3; ++i )
            for ( int j = 0; j < 3; ++j )
                std::cout << "Tuple " << t << ", member 0 element (" << i << ","
                          << j << "): " << Cabana::get<0>( tp, i, j )
                          << std::endl;
        for ( int i = 0; i < 4; ++i )
            std::cout << "Tuple " << t << ", member 1 element (" << i
                      << "): " << Cabana::get<1>( tp, i ) << std::endl;

        int value = Cabana::get<2>( tp );
        int expected = ( t == 3 ? 3 : 1234 + ( t % VectorLength ) ) + 1000;
        std::cout << "Tuple " << t << ", member 2: " << value
                  << " (expected " << expected << ")" << std::endl;
        if ( value != expected )
            ok = false;
    }

    std::cout << ( ok ? "\nPASS" : "\nFAIL" ) << std::endl;
    return ok;
}

//---------------------------------------------------------------------------//
// Main.
//---------------------------------------------------------------------------//
int main( int argc, char* argv[] )
{
    Kokkos::ScopeGuard scope_guard( argc, argv );

    bool ok = aosoaExample();

    return ok ? 0 : 1;
}
