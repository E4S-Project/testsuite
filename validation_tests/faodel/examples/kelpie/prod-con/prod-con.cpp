// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

// Warning: This example currently currently has problems at shutdown
//          when operations are still in flight.

#include <chrono>
#include <memory>
#include <vector>
#include <iostream>
#include <fstream>
#include <functional>
#include <numeric>
#include <stdexcept>

#include <mpi.h>

#include "boost/program_options.hpp"

#ifdef HAVE_HDF5
#include "H5Cpp.h"
#endif

#include "faodel-common/Common.hh"
#include "lunasa/Lunasa.hh"
#include "opbox/OpBox.hh"
#include "dirman/DirMan.hh"
#include "kelpie/Kelpie.hh"

#include "whookie/Server.hh"

#include "Globals.hh"

namespace po = boost::program_options;
using hrclock = std::chrono::high_resolution_clock;

Globals G;

size_t data_size;
size_t item_count;
size_t np, nc;
uint32_t ratio;
size_t participants;

faodel::ResourceURL url("dht:/prod-con");
kelpie::Pool dht;

std::string config_string = R"EOS(

dirman.root_role rooter
dirman.type centralized

target.dirman.host_root

# MPI tests will need to have a standard networking base
kelpie.type standard

#bootstrap.debug true
#whookie.debug true
#opbox.debug true
#dirman.debug true
#kelpie.debug true

)EOS";


bool text_output;

#ifdef HAVE_HDF5
bool hdf5_output;
H5std_string h5_prefix = "";
#endif

bool verbose;

std::vector <uint64_t> key_elapsed, act_elapsed;

template<typename F>
uint64_t time_it(F &&f) {
  auto start = hrclock::now();
  f();
  auto end = hrclock::now();
  return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

void produce(size_t ds) {
  for(size_t i = 0; i<item_count; i++) {

    kelpie::Key k;

    key_elapsed.push_back(time_it([&] {
        k.K1(std::to_string(G.mpi_rank));
        k.K2(std::to_string(i));
    }));

    lunasa::DataObject ldo(0, ds, lunasa::DataObject::AllocatorType::eager);

    act_elapsed.push_back(time_it([&] {
        dht.Publish(k, ldo);
    }));

    if(verbose) {
      std::cout << "Publishing "
                << k.str()
                << std::endl;
    }

  }

  if(verbose) std::cerr << "producer rank " << G.mpi_rank << " finished" << std::endl;
}


void consume(size_t ds) {
  /* ratio:1 producers to consumers, so each time we have to get |ratio| things */

  for(size_t r = 1; r<=ratio; r++) {

    for(size_t j = 0; j<item_count; j++) {

      kelpie::Key k;

      key_elapsed.push_back(time_it([&] {
          k.K1(std::to_string((nc*r) + G.mpi_rank));
          k.K2(std::to_string(j));
      }));

      lunasa::DataObject ldo1;

      if(verbose) std::cout << "Consumer requesting " << k.str() << std::endl;
      act_elapsed.push_back(time_it([&] {
          dht.Need(k, &ldo1);
      }));
      if(verbose) {
        std::cout << "Consumer consumed "
                  << k.str()
                  << std::endl;
      }

    }
  }
  if(verbose) {
    std::cout << "consumer rank " << G.mpi_rank << " finished" << std::endl;
  }
}


int main(int argc, char **argv) {

  try {

    po::options_description opt_desc("prod-con options");
    po::variables_map opt_map;

    opt_desc.add_options()
            ("help,?", "This menu")
            ("ratio,r", po::value<uint32_t>(&ratio)->default_value(1), "Ratio of producers to consumers")
            ("count,c", po::value<size_t>(&item_count)->default_value(10),
             "Number of items each producer should create")
            ("size,s", po::value<size_t>(&data_size)->default_value(32768), "Size of element producers should create")
            ("verbose,v", po::bool_switch(&verbose)->default_value(false), "Be chatty")
            ("text-output,t", po::bool_switch(&text_output)->default_value(false), "Write ASCII output")
#ifdef HAVE_HDF5
      ( "hdf5-output,h", po::bool_switch( &hdf5_output )->default_value( false ), "Write HDF5 output" )
#endif
            ;

    po::store(po::parse_command_line(argc, argv, opt_desc), opt_map);
    po::notify(opt_map);

    faodel::Configuration config(config_string);
    config.AppendFromReferences();

    G.StartAll(argc, argv, config);

    if(opt_map.count("help")) {
      if(G.mpi_rank == 0)
        std::cout << opt_desc << std::endl;
      G.StopAll();
      return 0;
    }

    participants = G.mpi_size;

    /* Enforce 2:1 ratio of producers to consumers */
    if(participants%(ratio + 1)) {
      throw (std::runtime_error(std::string("Stopping: number of ranks ")
                                + "(" + std::to_string(participants) + ")"
                                + " incompatible with ratio "
                                + "(" + std::to_string(ratio) + ")"));
    }


    //We use the dirman service to define what nodes are in a DHT. We can
    //either have individual nodes volunteer to be in the DHT or just have
    //one node register everyone. Do a barrier here so the registration
    //happens before anyone tries to use it.
    if(G.mpi_rank == 0) {
      faodel::DirectoryInfo dirinfo("dht:/prod-con", "prod-con-DHT");
      if(verbose) std::cerr << "Created dirinfo" << std::endl;
      for(size_t s = 0; s<participants; s++)
        dirinfo.Join(G.nodes[s]);
      dirman::HostNewDir(dirinfo);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    auto connect_time = time_it([&] { dht = kelpie::Connect(url); });

    nc = participants/(ratio + 1);
    np = nc*ratio;

    uint64_t total_time;

    if(G.mpi_rank<nc) {
      total_time = time_it([&] { consume(data_size); });
    } else {
      total_time = time_it([&] { produce(data_size); });
    }

    // calculate statistics
    double mean = std::accumulate(act_elapsed.begin(), act_elapsed.end(), 0.0)/act_elapsed.size();

    auto n = act_elapsed.size()/2;
    std::nth_element(act_elapsed.begin(), act_elapsed.begin() + n, act_elapsed.end());
    auto median = act_elapsed[n];
    if(!(act_elapsed.size() & 1)) { // even size
      auto max_it = std::max_element(act_elapsed.begin(), act_elapsed.begin() + n);
      median = (*max_it + median)/2.0;
    }

#ifdef HAVE_HDF5

    if( hdf5_output ) {

      H5::Exception::dontPrint();
      std::unique_ptr< H5::H5File > hfile( new H5::H5File() );

      std::string fname( std::to_string( G.mpi_rank ) + ".h5" );
      try {
    hfile->openFile( fname, H5F_ACC_TRUNC );
      }
      catch( H5::FileIException& xc ) {
    // above open failed, file must not exists, re-create RDWR w/ exclusive access
    hfile.reset( new H5::H5File( fname, H5F_ACC_TRUNC ) );
      }

      try {
    using namespace H5;

    hsize_t dim[1];
    dim[0] = act_elapsed.size();
    H5::DataSpace dspace( 1, dim );

    h5_prefix = "prod-con";

    std::unique_ptr< H5::Group > group ( new H5::Group( hfile->createGroup( h5_prefix ) ) );

    std::unique_ptr< H5::DataSet > key_dset ( new H5::DataSet( hfile->createDataSet( h5_prefix + "/key-elapsed",
                                             PredType::NATIVE_UINT64, dspace ) ) );
    key_dset->write( key_elapsed.data(), PredType::NATIVE_UINT64 );

    std::unique_ptr< H5::DataSet > act_dset ( new H5::DataSet( hfile->createDataSet( h5_prefix + "/act-elapsed",
                                             PredType::NATIVE_UINT64, dspace ) ) );
    act_dset->write( act_elapsed.data(), PredType::NATIVE_UINT64 );

    DataSpace att_space( H5S_SCALAR );
    H5::Attribute att1 = act_dset->createAttribute( "mean", PredType::NATIVE_DOUBLE, att_space );
    att1.write( PredType::NATIVE_FLOAT, &mean );
    H5::Attribute att2 = act_dset->createAttribute( "median", PredType::NATIVE_UINT64, att_space );
    att2.write( PredType::NATIVE_ULONG, &median );
    H5::Attribute att3 = act_dset->createAttribute( "np", PredType::NATIVE_ULONG, att_space );
    att3.write( PredType::NATIVE_ULONG, &np );
    H5::Attribute att4 = act_dset->createAttribute( "nc", PredType::NATIVE_ULONG, att_space );
    att4.write( PredType::NATIVE_ULONG, &nc );
    H5::Attribute att5 = act_dset->createAttribute( "ratio", PredType::NATIVE_UINT32, att_space );
    att5.write( PredType::NATIVE_ULONG, &ratio );
    H5::Attribute att6 = act_dset->createAttribute( "total-time", PredType::NATIVE_UINT64, att_space );
    att6.write( PredType::NATIVE_ULONG, &total_time );

      }
      catch( const H5::Exception& xc ) {
/*
 H5::Exception::printError() was removed in a maintenance release
 of both the v1.8.x and v1.10.x series.  We try to cope here.
*/
#if (H5_VERS_MAJOR == 1)
 #if (H5_VERS_MINOR < 8) || \
     ((H5_VERS_MINOR == 8) && (H5_VERS_RELEASE < 20)) || \
     ((H5_VERS_MINOR == 10) && (H5_VERS_RELEASE < 2))
    // this is the old API
    xc.printError();
 #else
    // this is the new API
    xc.printErrorStack();
 #endif
#else
 #warning In HDF5 version 0.x, exception stack dumps are unsupported.  No messages will be printed if an exception occurs.
#endif
      }
    }

#endif

    if(text_output) {

      std::string rank_as_str = std::to_string(G.mpi_rank);

      std::ofstream ke(rank_as_str + "-key-elapsed.txt");
      for(auto &&k : key_elapsed)
        ke << k << std::endl;

      std::ofstream ae(rank_as_str + "-act-elapsed.txt");
      for(auto &&a : act_elapsed)
        ae << a << std::endl;

      std::ofstream attrs(rank_as_str + "-attrs.txt");
      attrs << "mean " << mean << std::endl
            << "median " << median << std::endl
            << "np " << np << std::endl
            << "nc " << nc << std::endl
            << "ratio " << ratio << std::endl
            << "total-time " << total_time << std::endl;
    }

    G.StopAll();
  }
  catch(const po::unknown_option &uo) {
    // This will have been thrown before MPI_Init, so no MPI stuff
    std::cerr << uo.what() << std::endl;
    return 1;
  }
  catch(const std::exception &x) {
    if(G.mpi_rank == 0)
      std::cerr << x.what() << std::endl;
    G.StopAll();
    return 1;
  }

  return 0;
}
