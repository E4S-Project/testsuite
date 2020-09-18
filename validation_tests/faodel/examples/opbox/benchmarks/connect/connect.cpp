// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 


#include <mpi.h>

#include <chrono>

#include <boost/program_options.hpp>

#include "faodel-common/Common.hh"
#include "opbox/OpBox.hh"

using namespace std;
using namespace faodel;


Configuration config;
int mpi_rank, mpi_size;
int root_rank;

uint32_t connect_count;

struct connect_timer {
    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point end;
};
std::vector<struct connect_timer> *timers;

void
connect_1to1(uint32_t count)
{
    int rc;

    //std::cout << "Our MPI rank is " << mpi_rank << std::endl;
    faodel::nodeid_t myid = opbox::GetMyID();
    //std::cout << "Our nodeid is " << myid.GetHex() << std::endl;

    std::vector<faodel::nodeid_t> gather_result(mpi_size);
    MPI_Allgather(
        &myid,
        sizeof(faodel::nodeid_t),
        MPI_CHAR,
        gather_result.data(),
        sizeof(faodel::nodeid_t),
        MPI_CHAR,
        MPI_COMM_WORLD);

    if (mpi_rank == root_rank) {
    } else if (mpi_rank == root_rank+1) {
        opbox::net::peer_t *peer;
        faodel::nodeid_t root_nodeid = gather_result[root_rank];
        for(int i = 0; i<count; i++) {
            timers->at(i).start = std::chrono::high_resolution_clock::now();
            rc = opbox::net::Connect(&peer, root_nodeid);
            timers->at(i).end = std::chrono::high_resolution_clock::now();
            rc = opbox::net::Disconnect(root_nodeid);
        }
        for(int i = 0; i<count; i++) {
          uint64_t end = std::chrono::duration_cast<std::chrono::microseconds>(
                  timers->at(i).end.time_since_epoch()).count();
          uint64_t start = std::chrono::duration_cast<std::chrono::microseconds>(
                  timers->at(i).start.time_since_epoch()).count();
          uint64_t per_connect_us = end - start;
          fprintf(stdout, "1-to-1 Connect() chrono - %6lu   %6lu   %6lu    %6luus\n",
                  i, start, end, per_connect_us);
        }
        uint64_t total_us = std::chrono::duration_cast<std::chrono::microseconds>(
                timers->at(count - 1).end - timers->at(0).start).count();
        float total_sec = (float) total_us/1000000.0;
        fprintf(stdout, "1-to-1 Connect() chrono - total - %6lu    %6luus   %6.6fs\n",
                count, total_us, total_sec);
    } else {
    }
    MPI_Barrier(MPI_COMM_WORLD);
}

void
connect_nto1(uint32_t count)
{
    int rc;

    //std::cout << "Our MPI rank is " << mpi_rank << std::endl;
    faodel::nodeid_t myid = opbox::GetMyID();
    //std::cout << "Our nodeid is " << myid.GetHex() << std::endl;

    std::vector<faodel::nodeid_t> gather_result(mpi_size);
    MPI_Allgather(
        &myid,
        sizeof(faodel::nodeid_t),
        MPI_CHAR,
        gather_result.data(),
        sizeof(faodel::nodeid_t),
        MPI_CHAR,
        MPI_COMM_WORLD);

    if(mpi_rank == root_rank) {
    } else {
        opbox::net::peer_t *peer;
        faodel::nodeid_t root_nodeid = gather_result[root_rank];
        for(int i = 0; i<count; i++) {
            timers->at(i).start = std::chrono::high_resolution_clock::now();
            rc = opbox::net::Connect(&peer, root_nodeid);
            timers->at(i).end = std::chrono::high_resolution_clock::now();
            rc = opbox::net::Disconnect(root_nodeid);
        }
        for(int i = 0; i<count; i++) {
          uint64_t end = std::chrono::duration_cast<std::chrono::microseconds>(
                  timers->at(i).end.time_since_epoch()).count();
          uint64_t start = std::chrono::duration_cast<std::chrono::microseconds>(
                  timers->at(i).start.time_since_epoch()).count();
          uint64_t per_connect_us = end - start;
          fprintf(stdout, "N-to-1 Connect() chrono - %6lu   %6lu   %6lu    %6luus\n",
                  i, start, end, per_connect_us);
        }
        uint64_t total_us = std::chrono::duration_cast<std::chrono::microseconds>(
                timers->at(count - 1).end - timers->at(0).start).count();
        float total_sec = (float) total_us/1000000.0;
        fprintf(stdout, "N-to-1 Connect() chrono - total - %6lu    %6luus   %6.6fs\n",
                count, total_us, total_sec);
    }
    MPI_Barrier(MPI_COMM_WORLD);
}

void
connect_1ton(uint32_t count)
{
    int rc;

    //std::cout << "Our MPI rank is " << mpi_rank << std::endl;
    faodel::nodeid_t myid = opbox::GetMyID();
    //std::cout << "Our nodeid is " << myid.GetHex() << std::endl;

    std::vector<faodel::nodeid_t> gather_result(mpi_size);
    MPI_Allgather(
        &myid,
        sizeof(faodel::nodeid_t),
        MPI_CHAR,
        gather_result.data(),
        sizeof(faodel::nodeid_t),
        MPI_CHAR,
        MPI_COMM_WORLD);

    if(mpi_rank == root_rank) {
        for(int i=0; i<count; i++) {
            for (int j=1; j<mpi_size; j++) {
                opbox::net::peer_t *peer;
                faodel::nodeid_t nodeid = gather_result[j];
                timers->at(i).start = std::chrono::high_resolution_clock::now();
                rc = opbox::net::Connect(&peer, nodeid);
                timers->at(i).end = std::chrono::high_resolution_clock::now();
                rc = opbox::net::Disconnect(nodeid);
            }
        }
        for(int i = 0; i<count; i++) {
          uint64_t end = std::chrono::duration_cast<std::chrono::microseconds>(
                  timers->at(i).end.time_since_epoch()).count();
          uint64_t start = std::chrono::duration_cast<std::chrono::microseconds>(
                  timers->at(i).start.time_since_epoch()).count();
          uint64_t per_connect_us = end - start;
          fprintf(stdout, "1-to-N Connect() chrono - %6lu   %6lu   %6lu    %6luus\n",
                  i, start, end, per_connect_us);
        }
        uint64_t total_us = std::chrono::duration_cast<std::chrono::microseconds>(
                timers->at(count - 1).end - timers->at(0).start).count();
        float total_sec = (float) total_us/1000000.0;
        fprintf(stdout, "1-to-N Connect() chrono - total - %6lu    %6luus   %6.6fs\n",
                count, total_us, total_sec);
    } else {
    }
    MPI_Barrier(MPI_COMM_WORLD);
}

int main(int argc, char **argv) {
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);

    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
            ("help", "Show this help message")
            ("count",    boost::program_options::value<uint32_t>(), "number of connects to issue")
            ;

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
    boost::program_options::notify(vm);

    if (vm.count("help")) {
        cout << desc << "\n";
        return 1;
    }

    connect_count = 25;
    if (vm.count("count")) {
        connect_count = vm["count"].as<uint32_t>();
    }

    //This is a simple example of how to launch a small ping-pong communication
    cout <<"Connect Benchmark (" << connect_count << ")" << endl;

    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);

    bootstrap::Start(Configuration(""), opbox::bootstrap);

    timers = new std::vector<struct connect_timer>(connect_count);

    root_rank=0;
    MPI_Barrier(MPI_COMM_WORLD);

    connect_1to1(connect_count);
    MPI_Barrier(MPI_COMM_WORLD);

    connect_1ton(connect_count);
    MPI_Barrier(MPI_COMM_WORLD);

    connect_nto1(connect_count);
    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    bootstrap::Finish();

    MPI_Finalize();
    return 0;
}
