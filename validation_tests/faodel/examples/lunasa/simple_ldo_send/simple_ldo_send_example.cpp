// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 

#include <iostream>
#include <assert.h>
#include <mpi.h>

#include "faodel-common/Common.hh"
#include "lunasa/Lunasa.hh"
#include "lunasa/DataObject.hh"

#include "faodel-common/Configuration.hh"
#include "faodel-common/NodeID.hh"

#include "nnti/nnti.h"
#include "nnti/nnti_logger.hpp"
#include "nnti/nnti_transport.hpp"
#include "nnti/nnti_callback.hpp"
#include "nnti/nnti_wr.hpp"
#include "nnti/transport_factory.hpp"
#include "whookie/Server.hh"

#include <time.h>
#include <future>
#include <set>

#include <assert.h>

using namespace std;

const int URL_SIZE=128;

string default_config = R"EOF(

server.mutex_type rwlock

# In this example, the default allocator is lunasa::AllocatorTcmalloc
lunasa.eager_memory_manager tcmalloc
node_role server
)EOF";

/* -----------------------
 * Simple LDO Send EXAMPLE
 * ------------------------------------------------------------------
 *  This example demonstrates how an LDO can be transmitted from one 
 *  process to another using the services provided by NNTI.
 * ------------------------------------------------------------------
 */

/* NOTE: for the purposes of this example, I've expressly omitted "using namespace ..." statements
 *       to help identify the source of each symbol. [sll]
 */

/* We use eagerly-allocated memory (i.e., guaranteed to be registered with
 * the network at allocation time) to store our message */
faodel::Configuration config;
int mpi_rank,mpi_size;

nnti::transports::transport *transport = nullptr;
NNTI_event_queue_t  unexpected_eq_;

/* Objects that allow us to asynchronously share state information between execution contexts */
std::promise<int> send_promise, recv_promise;
std::future<int> send_future, recv_future;

class send_callback_functor
{
  private:
    std::promise<int> &send_promise;
  public:
    send_callback_functor(std::promise<int> &send_promise_):send_promise(send_promise_) 
    {}

    NNTI_result_t operator() (NNTI_event_t *event, void *context)
    {
        std::cout << "SEND is complete" << std::endl;
        send_promise.set_value(1);
        return NNTI_EIO;
    }
};

class unexpected_callback
{   
  private:
    nnti::transports::transport *transport = nullptr;
    std::promise<int>& recv_promise;

    NNTI_result_t recv_callback_func(NNTI_event_t *event, void *context)
    {
      std::cout << "RECEIVE is complete" << std::endl;
        
      NNTI_attrs_t nnti_attrs;
      transport->attrs(&nnti_attrs);
      
      lunasa::DataObject memory(nnti_attrs.mtu-nnti_attrs.max_eager_size, nnti_attrs.max_eager_size, 
                                lunasa::DataObject::AllocatorType::eager);
      NNTI_buffer_t rdma_buffer;
      uint32_t rdma_offset = 0;
      std::queue<lunasa::DataObject::rdma_segment_desc> rdma_segments;
      memory.GetDataRdmaHandles(rdma_segments);
      assert(rdma_segments.size() == 1); 
      lunasa::DataObject::rdma_segment_desc rdma_segment = rdma_segments.front();
      rdma_buffer = (NNTI_buffer_t)rdma_segment.net_buffer_handle;
      rdma_offset = rdma_segment.net_buffer_offset;

      NNTI_event_t e;
      transport->next_unexpected(rdma_buffer, rdma_offset, &e);

      char *msg = ((char*)e.start + e.offset);
      // FORCE ordering of msg increment to silence compiler warning
      int length = (*msg++ & 0xff) << 8;
      length |= (*msg++ & 0xff);
      assert(length > 0);
      // FORCE ordering of msg increment to silence compiler warning
      int seed = (*msg++ & 0xff) << 8;
      seed |= (*msg++ & 0xff);

      std::srand(seed);
      for( int i = 0; i < length; i++ ) {
          char expected = (std::rand() & 0xff);
          char received = (*msg++ & 0xff);
          assert(received == expected);
      }
      
      recv_promise.set_value(1);
      return NNTI_EIO;
    }

  public:
      unexpected_callback(nnti::transports::transport *transport_, std::promise<int>& recv_promise_) : 
          transport(transport_),  recv_promise(recv_promise_)
      {}

      NNTI_result_t operator() (NNTI_event_t *event, void *context)
      {
          recv_callback_func(event, context);
          return NNTI_OK;
      }
};

/* 
 * Lunasa does not have explicit knowledge about how to register memory with the network transport.
 * This functor (register_memory_func) contains that knowledge for registering memory with an
 * NNTI transport.  We'll pass it to Lunasa below.
 */ 
class register_memory_func {
  private:
    nnti::transports::transport *transport = nullptr;

  public:
    register_memory_func(nnti::transports::transport *transport_) : transport(transport_)
    {}

    void operator() (void *base_addr, size_t length, void *&pinned)
    {
      NNTI_buffer_t reg_buf;
      nnti::datatype::nnti_event_callback null_cb(transport, (NNTI_event_callback_t)0);

      transport->register_memory(
          (char *)base_addr,
          length,
          NNTI_BF_LOCAL_WRITE,
          (NNTI_event_queue_t)0,
          null_cb,
          nullptr,
          &reg_buf);
      pinned = (void*)reg_buf;
    }
};

/* 
 * Same story for de-registration.  Lunasa does not have explicit knowledge about how to tell the
 * network transport to de-register memory.  This functor (register_memory_func) contains that 
 * knowledge for de-registering memory with an NNTI transport.  We'll pass it to Lunasa below.
 */ 
class unregister_memory_func {
  private:
    nnti::transports::transport *transport = nullptr;

  public:
    unregister_memory_func(nnti::transports::transport *transport_) : transport(transport_)
    {}

    void operator() (void *&pinned)
    {
      NNTI_buffer_t reg_buf = (NNTI_buffer_t)pinned;
      transport->unregister_memory(reg_buf);
      pinned = nullptr;
    }
};

int main(int argc, char **argv){
  int rc;
  faodel::nodeid_t nodeid = faodel::NODE_UNSPECIFIED;

  cout <<"Starting Bootstrap example's main()\n";

  //Configuration allows us to describe configuration info for different
  //components in plain text. A Configuration object holds values in
  //a simple key/value notation that the components can pull out.
  config = faodel::Configuration(default_config);

  /* 
   * We specified a default configuration (default_config) at compile time.  It is also 
   * possible to modify * the configuration at runtime.  The value of 
   * config.additional_files.env_name.if_defined specifies the name of an environment 
   * variable that will, in turn, contain the location of the runtime configuration.  
   * AppendFromReferences() updates our Configuration object based on the runtime 
   * configuration (if any).
   */
  config.AppendFromReferences();

  //There are two steps to initializing the components: Init and 
  //Start. The Init sets up basic configuration, but leaves the
  //component in a state where later components can update its 
  //settings. The Start option is used to officially start the
  //component. After this point the service cannot be modified.
  faodel::bootstrap::Init(config, lunasa::bootstrap);
  faodel::bootstrap::Start();

  {
    send_future = send_promise.get_future();
    recv_future = recv_promise.get_future();

    /* 
     * Initialize MPI.  We are going to use MPI to exchange information between our 
     * two processes that we need to configure the NNTI transfer.
     */
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);

    /* Currently, only works for two MPI ranks: one sender and one receiver. */
    assert(2==mpi_size);

    /* 
     * The value of <nodeid> allows us to tell NNTI which process we want to connect to.
     * We get our ID here so that we can send it to our partner process.
     */
    assert(whookie::Server::IsRunning() && "Whookie not started before NetNnti started");
    nodeid = whookie::Server::GetNodeID();

    /* Initialize and configure our NNTI transport */
    transport = nnti::transports::factory::get_instance(config);
    transport->start();

    /* REGISTER the functor that will be invoked when a message is received. */
    nnti::datatype::nnti_event_callback recv_cb(transport, unexpected_callback(transport, recv_promise));
    NNTI_result_t result = transport->eq_create(128, NNTI_EQF_UNEXPECTED, recv_cb, nullptr, &unexpected_eq_);
    assert(result == NNTI_OK);

    /* Tell Lunasa how to register and de-register memory with NNTI */
    lunasa::RegisterPinUnpin(register_memory_func(transport), unregister_memory_func(transport));

    /* 
     * Currently, this example only works for two MPI ranks.  So it's straightforward
     * to determine the rank of the other process.
     */
    int partner_rank = mpi_rank ^ 0x1;

    /* Exchange NodeIDs with our partner. */
    faodel::nodeid_t partner_nodeid; 
    MPI_Request request;
    rc = MPI_Isend((void *)&nodeid, sizeof(faodel::nodeid_t), MPI_CHAR, partner_rank, 0, MPI_COMM_WORLD, &request);
    assert(rc == MPI_SUCCESS);

    MPI_Status status;
    rc = MPI_Recv((void *)&partner_nodeid, sizeof(faodel::nodeid_t), MPI_CHAR, partner_rank, 0, MPI_COMM_WORLD, &status);
    assert(rc == MPI_SUCCESS);

    /* We use the MPI rank of each process to determine roles: rank 0 is SENDER, rank 1 is RECEIVER */
    if( mpi_rank < partner_rank ) {
        /* Because there are only two MPI ranks, if we're here then we're the SENDER */
        NNTI_peer_t p;

        transport->connect(partner_nodeid.GetHttpLink().c_str(), 1000, &p);
        int payload_length = 32;

        /* Allocate memory to store the message that we're going to send to our partner. */
        NNTI_attrs_t nnti_attrs;
        transport->attrs(&nnti_attrs);
        lunasa::DataObject memory(nnti_attrs.mtu-nnti_attrs.max_eager_size, payload_length+4,
                                  lunasa::DataObject::AllocatorType::eager);

        /* 
         * The payload we're going to send is: | length | seed | list of random numbers |
         * Using the seed, the receiver can reconstruct the sequence of random numbers 
         * with its own pseudo-random number generator.
         */
        char *payload = (char *)memory.GetDataPtr();
        *payload++ = (payload_length >> 8) & 0xff;
        *payload++ = payload_length & 0xff;

        unsigned int seed = std::time(0) & 0xffff;
        *payload++ = (seed >> 8) & 0xff;
        *payload++ = seed & 0xff;

        std::srand(seed);
        for( int i = 0; i < payload_length; i++ ) {
            char p = std::rand() & 0xff;
            *payload++ = p;
        }

        /* Construct the SEND request that we will deliver to NNTI. */
        NNTI_work_request_t base_wr = NNTI_WR_INITIALIZER;
        NNTI_work_id_t      wid;

        NNTI_buffer_t rdma_buffer;
        uint32_t rdma_offset = 0;
        memory.GetMetaRdmaHandle((void **)&rdma_buffer, rdma_offset);
        std::queue<lunasa::DataObject::rdma_segment_desc> rdma_segments;
        memory.GetMetaRdmaHandles(rdma_segments);
        assert(rdma_segments.size() == 1); 

        lunasa::DataObject::rdma_segment_desc rdma_segment = rdma_segments.front();
        rdma_buffer = (NNTI_buffer_t)rdma_segment.net_buffer_handle;
        rdma_offset = rdma_segment.net_buffer_offset;

        base_wr.op            = NNTI_OP_SEND;
        base_wr.flags         = (NNTI_op_flags_t)(NNTI_OF_LOCAL_EVENT | NNTI_OF_ZERO_COPY);
        base_wr.trans_hdl     = nnti::transports::transport::to_hdl(transport);
        base_wr.peer          = p;
        base_wr.local_hdl     = rdma_buffer;
        base_wr.local_offset  = rdma_offset;
        base_wr.remote_hdl    = NNTI_INVALID_HANDLE;
        base_wr.remote_offset = 0;
        base_wr.length        = memory.GetMetaSize() + memory.GetDataSize();
        nnti::datatype::nnti_event_callback send_callback(transport, send_callback_functor(send_promise));
        nnti::datatype::nnti_work_request wr(transport, base_wr, send_callback);

        /* SEND the message. */
        transport->send(&wr, &wid);

        /* Wait for the send to complete. */
        send_future.get();
    } else {
        /* Because we know that there are only two MPI ranks, if we're here then we're the SENDER */

        /* WAIT until we have received the message from the SENDER. */
        recv_future.get();
    }
  }

  if( transport->initialized() ) {
    transport->stop();
  }
  MPI_Finalize();

  //When all work is done, call the Finish command. This stops all
  //components and deallocates space.
  faodel::bootstrap::Finish();

  cout <<"Exiting Simple LDO example\n";
}
