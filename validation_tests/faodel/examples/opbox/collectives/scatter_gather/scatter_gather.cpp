// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#include <iostream>
#include <cstring>

#include "faodel-common/Common.hh"
#include "opbox/OpBox.hh"

#include "Globals.hh"
#include "SimpleDataStore.hh"
#include "OpScatter.hh"
#include "OpGather.hh"

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

#nnti.logger.severity debug

)EOF";

using namespace std;


lunasa::DataObject createScatterSource(uint32_t block_size, uint32_t block_count, std::string fill_str)
{
    // allocate an Lunasa DataObject (LDO) to be the target of RDMA operations
    lunasa::DataObject ldo = lunasa::DataObject(0, block_count * block_size, lunasa::DataObject::AllocatorType::eager);
    memset((char*)ldo.GetDataPtr(), 0, ldo.GetDataSize());

    for (int i=0;i<block_count;i++) {
        uint32_t offset = block_size * i;
        std::stringstream ss;
        ss << fill_str+to_string(i);
        // copy the message into the RDMA target
        memcpy(ldo.GetDataPtr<char *>() + offset, ss.str().c_str(), strlen(ss.str().c_str()));
    }

    return ldo;
}

lunasa::DataObject createGatherDestination(uint32_t block_size, uint32_t block_count)
{
    // allocate an Lunasa DataObject (LDO) to be the target of RDMA operations
    lunasa::DataObject ldo = lunasa::DataObject(0, block_count * block_size, lunasa::DataObject::AllocatorType::eager);
    memset(ldo.GetDataPtr<char *>(), 0, ldo.GetDataSize());

    return ldo;
}

int main(int argc, char **argv){

  //This is a simple example of how to launch a small RDMA ping-pong communication
  cout <<"My simple RDMA scatter/gather example\n";

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
  opbox::RegisterOp<OpGather>();
  opbox::RegisterOp<OpScatter>();

  //Next, we want to create a master config that tells all the faodel
  //components how they should be configured. For the examples, the Global
  //class modifies the config so it can designate rank 0 as the master and
  //any other nodes as servers.
  faodel::Configuration config(default_config_string);
  config.AppendFromReferences();

  G.StartAll(argc, argv, config);

  //For this example, the master node is going to create a new Op and
  //launch it. The flow of operation is as follows:
  //  Origin (user)  : Allocate a Lunasa RDMA buffer and fill with data for each target
  //  Origin (user)  : Create a new OpScatter
  //  Origin (user)  : Get a future from the op in order to get the final result
  //  Origin (user)  : Hand over the op to opbox for execution
  //  Origin (opbox) : Create a message for each target, send it to the destination, wait for reply
  //  Target (opbox) : Observe a new message, create a new Op for processing it
  //  Target (Op)    : Extract key and NBR, RDMA get the data, store in SimpleDataStore
  //  Target (Op)    : Create ACK message, and send it to origin
  //  Target (opbox) : Destroy op, Destroy ACK message when sending completes
  //  Origin (opbox) : Observe a new message, match it to the op that sent it, increment ack count
  //  Origin (op)    : When all ACKs are received, set the future, tell opbox we're done
  //  Origin (user)  : Allocate a Lunasa RDMA buffer and fill with data from each target
  //  Origin (user)  : Create a new OpGather
  //  Origin (user)  : Get a future from the op in order to get the final result
  //  Origin (user)  : Hand over the op to opbox for execution
  //  Origin (opbox) : Create a message for each target, send it to the destination, wait for reply
  //  Target (opbox) : Observe a new message, create a new Op for processing it
  //  Target (Op)    : Extract key and NBR, retrieve from SimpleDataStore, RDMA putt the data
  //  Target (Op)    : Create ACK message, and send it to origin
  //  Target (opbox) : Destroy op, Destroy ACK message when sending completes
  //  Origin (opbox) : Observe a new message, match it to the op that sent it, increment ack count
  //  Origin (op)    : When all ACKs are received, set the future, tell opbox we're done
  //  Origin (opbox) : Destroy the op
  //  Origin (user)  : future available, provide result to user
  if (G.mpi_rank==0) {
    int target_count = G.mpi_size-1;

    /*
     * Setup Scatter
     */
    std::string scatter_str("scattered to rank");
    cout<<"Original base message is '"<<scatter_str<<"'"<<endl;

    uint32_t block_size = scatter_str.size()+10;

    lunasa::DataObject scatter_src_ldo = createScatterSource(block_size, target_count, scatter_str);

    /*
     * Start Scatter
     */
    // create the op.  all we need to tell it is the list of peers and
    // the LDO to scatter from.
    OpScatter *op1 = new OpScatter(&G.peers[1], target_count, scatter_src_ldo);
    // the op creates a future that we can use to wait for the op to complete
    future<int> scatter_future = op1->GetFuture();

    opbox::LaunchOp(op1);

    // when the op completes it will set the future to the transformed
    // ping message
    int scatter_result=scatter_future.get();

    cout<<"Scatter Result message is '"<<scatter_result<<"'\n";

    /*
     * Setup Gather
     */
    lunasa::DataObject gather_dst_ldo = createGatherDestination(block_size, target_count);

    /*
     * Start Gather
     */
    // create the op.  all we need to tell it is the list of peer and
    // the LDO to gather into.
    OpGather *op2 = new OpGather(&G.peers[1], target_count, gather_dst_ldo);
    // the op creates a future that we can use to wait for the op to complete
    future<int> gather_future = op2->GetFuture();

    opbox::LaunchOp(op2);

    // when the op completes it will set the future to the transformed
    // ping message
    int gather_result=gather_future.get();

    cout<<"Gather Result message is '" << gather_result << "'" << std::endl;

    for (int i=0;i<target_count;i++) {
        uint32_t offset = block_size * i;
        std::string s = std::string(gather_dst_ldo.GetDataPtr<char *>() + offset, block_size);
        cout << s << endl;
    }
  }

  //Finally, do an mpi barrier to sync all nodes and then invoke shutdown procedures
  //to stop the FAODEL. Global also does an mpi finalize to close out the test.
  G.StopAll();

  return 0;
}
