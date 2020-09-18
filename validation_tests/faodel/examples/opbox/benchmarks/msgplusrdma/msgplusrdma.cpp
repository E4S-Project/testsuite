// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#include <unistd.h>
#include <iostream>

#include <boost/program_options.hpp>

#include "faodel-common/Common.hh"
#include "opbox/OpBox.hh"

#include "Globals.hh"
#include "OpBenchmarkMsgRdma.hh"

//The Globals class just holds basic communication vars we use in these
//examples (ie mpi ranks, etc). It has a generic hook for starting/stopping
//all nodes in this mpi run to make the OpBox codes easier to understand.
Globals G;


//The FAODEL uses a plain-text configuration string to set
//different parameters in the stack. The below string defines what
//port the whookie sever will listen on, the type of directory management
//service to employ, and whether internal components should spew their
//debug information or not. If things don't work, try turning on the debug
//info to get a better idea of where things are breaking

std::string default_config_string = R"EOF(
# Note: node_role is defined when we determine if this is a client or a server

master.whookie.port   7777
server.whookie.port   1992

dirman.type           centralized
dirman.root_role      master

#target.dirman.host_root
#target.dirman.write_to_file ./dirman.txt

#bootstrap.debug true
#whookie.debug   true
#opbox.debug     true
#dirman.debug    true

)EOF";

using namespace std;


int main(int argc, char **argv)
{
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
        ("help", "Show this help message")
        ("count",    boost::program_options::value<uint32_t>(), "number of RDMAs to issue")
        ("inflight", boost::program_options::value<uint32_t>(), "number of outstanding RDMAs allowed")
        ("length",   boost::program_options::value<uint32_t>(), "length of each RDMA (>=8)")
    ;

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
    boost::program_options::notify(vm);

    if (vm.count("help")) {
        cout << desc << "\n";
        return 1;
    }

    uint32_t count = 25;
    if (vm.count("count")) {
        count = vm["count"].as<uint32_t>();
    }

    uint32_t inflight = 10;
    if (vm.count("inflight")) {
        inflight = vm["inflight"].as<uint32_t>();
    }

    uint32_t length = 128;
    if (vm.count("length")) {
        length = vm["length"].as<uint32_t>();
    }

    //This is a simple example of how to launch a small ping-pong communication
    cout <<"Msg+RDMA Benchmark (" << count << ", " << inflight << ", " << length << ")" << endl;

    //First, we need to register our new Op so OpBox will know how to handle a
    //particular Op whenever it sees a user request or message relating to it.
    //The RegisterOp template examines the class you give it by creating and
    //examining a dummy instance of the class to see what its id/name are.
    //
    //Ideally, we want to do registration *before* we start up opbox because
    //pre-init registrations can be maintained in a special list that doesn't
    //incur locking overheads at runtime. You *can* register ops after the
    //OpBox is started. However, those ops (1) incur a mutex and (2) won't be
    //known to OpBox until they're registered.
    opbox::RegisterOp<OpBenchmarkMsgRdma>();

    //Next, we want to create a master config that tells all the faodel
    //components how they should be configured. For the examples, the Global
    //class modifies the config so it can designate rank 0 as the master and
    //any other nodes as servers.
    faodel::Configuration config(default_config_string);
    config.AppendFromReferences();

    G.StartAll(argc, argv, config);

    //For this example, the master node is going to create a new Op and
    //launch it. The flow of operation is as follows:
    if(G.mpi_rank==0){
        OpBenchmarkMsgRdma *op = new OpBenchmarkMsgRdma(G.peers[1], length, count, inflight);
        future<uint32_t> fut = op->GetFuture();
        opbox::LaunchOp(op);
        uint32_t res=fut.get();
        cout<<"Second result is '"<<res<<"'\n";
    }

    //Finally, do an mpi barrier to sync all nodes and then invoke shutdown procedures
    //to stop the FAODEL. Global also does an mpi finalize to close out the test.
    G.StopAll();

    return 0;
}
