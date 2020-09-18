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
int mpi_rank, mpi_size, partner_rank;

nnti::transports::transport *transport = nullptr;
NNTI_event_queue_t  unexpected_eq_;

/* Objects that allow us to asynchronously share state information between execution contexts */
std::promise<int> send_promise, root_recv_promise, put_promise;
std::future<int> send_future, root_recv_future, put_future;
std::promise< std::pair<uint32_t, uint32_t> > leaf_recv_promise;
std::future< std::pair<uint32_t, uint32_t> > leaf_recv_future;

#if NNTI_BUILD_MPI
#define MAX_NET_BUFFER_REMOTE_SIZE 68 /* 4 + 4 + 60 */
#elif NNTI_BUILD_UGNI
#define MAX_NET_BUFFER_REMOTE_SIZE 48 /* 4 + 4 + 40 */
#elif NNTI_BUILD_IBVERBS
#define MAX_NET_BUFFER_REMOTE_SIZE 36 /* 4 + 4 + 28 */
#endif

// OVERVIEW
// This test is intended to verify that we can transmit LDOs using RDMA put operations.
// The ROOT process is the source of the put; the LEAF process is the destination of 
// the put (NOTE: in retrospect, calling them ROOT and SOURCE prob'ly would have made 
// more sense ;0)).  The principal mechanism that drives this test is a sequence of 
// NNTI callback functions.  The basic sequence of events is:
// 
// * [LEAF] : zero-copy SEND to ROOT advertising the HANDLE and OFFSET of the target memory
// * [ROOT] : receives message from LEAF, populates buffer using random seed and PRNG, PUTs 
//            the header of the source LDO 
// * [ROOT] : when PUT completes, the ROOT PUTs the remaining segment
// * [ROOT] : when all of the PUTs complete, the ROOT uses a zero-copy SEND to notify LEAF
//            of the random seed and that the PUT operations are complete.
// * [LEAF] : receives message from ROOT and validates the contents of the RDMA Put target

// --- DEFINE a message type for setting up the put ---
// We want a structure that is always MAX_NET_BUFFER_REMOTE_SIZE bytes long
typedef struct 
{
  uint32_t length;    // number of payload bytes
  uint32_t offset;    // offset in remote buffer
} start_put_message_header_t;

typedef struct 
{
  start_put_message_header_t header;
  char body[MAX_NET_BUFFER_REMOTE_SIZE - sizeof(start_put_message_header_t)];
} start_put_message_t;
static_assert(sizeof(start_put_message_t) == MAX_NET_BUFFER_REMOTE_SIZE,
	      "assert failed: sizeof(start_put_message_t) == MAX_NET_BUFFER_REMOTE_SIZE");

// --- DEFINE a message type for communicating that put is complete ---
typedef struct 
{
  uint32_t length;    // number of payload bytes in the put
  uint32_t seed;      // seed used to generate payload of the put
} completed_put_message_header_t;

typedef struct 
{
  completed_put_message_header_t header;
  char body[MAX_NET_BUFFER_REMOTE_SIZE - sizeof(completed_put_message_header_t)];
} completed_put_message_t;
static_assert(sizeof(completed_put_message_t) == MAX_NET_BUFFER_REMOTE_SIZE,
	      "assert failed: sizeof(completed_put_message_t) == MAX_NET_BUFFER_REMOTE_SIZE");

class put_status
{
  public:
    put_status(uint32_t remote_offset_, 
               std::queue<lunasa::DataObject::rdma_segment_desc> &rdma_segments_, 
               NNTI_buffer_t remote_hdl_, 
               uint32_t total_length_, 
               uint32_t seed_, 
               lunasa::DataObject ldo_) :
      remote_offset(remote_offset_),
      rdma_segments(rdma_segments_),
      remote_hdl(remote_hdl_),
      total_length(total_length_),
      seed(seed_),
      ldo(ldo_)
    {}
    uint32_t remote_offset;
    std::queue<lunasa::DataObject::rdma_segment_desc> rdma_segments;
    NNTI_buffer_t remote_hdl;
    uint32_t total_length;
    uint32_t seed;
    lunasa::DataObject ldo;
};

void cleanup(void *addr)
{
  free(addr);
}

  class send_callback_functor_root
  {
  private:
    std::promise<int> &send_promise;
  public:
    send_callback_functor_root(std::promise<int> &send_promise_) : send_promise(send_promise_) 
    {}

    NNTI_result_t operator() (NNTI_event_t *event, void *context)
    {
#ifdef DEBUG 
        std::cout << "This is a [root] SEND callback function.  My parameters are event(" << (void*)event 
                  << ") and context(" << (void*)context << ")" << std::endl;
#endif
        send_promise.set_value(1);
        return NNTI_EIO;
    }
};

class send_callback_functor_leaf
{
  private:
    std::promise<int> &send_promise;
  public:
    send_callback_functor_leaf(std::promise<int> &send_promise_):send_promise(send_promise_) 
    {}

    NNTI_result_t operator() (NNTI_event_t *event, void *context)
    {
#ifdef DEBUG 
        std::cout << "This is a [leaf] SEND callback function.  My parameters are event(" << (void*)event 
                  << ") and context(" << (void*)context << ")" << std::endl;
#endif
        send_promise.set_value(1);
        return NNTI_EIO;
    }
};

class put_callback_functor
{
  private:
    std::promise<int> &put_promise;
    std::promise<int> &send_promise;
    nnti::transports::transport *transport = nullptr;
    put_status *status;
  public:
    put_callback_functor(std::promise<int> &put_promise_, 
                         std::promise<int> &send_promise_,
                         nnti::transports::transport *transport_,
                         put_status *status_) :
      put_promise(put_promise_),
      send_promise(send_promise_),
      transport(transport_),
      status(status_)
    {}

    NNTI_result_t operator() (NNTI_event_t *event, void *context)
    {
#ifdef DEBUG 
      std::cout << "This is a PUT callback function.  My parameters are event(" << (void*)event 
                << ") and context(" << (void*)context << ")" << std::endl;
#endif
      NNTI_buffer_t rdma_buffer;
      uint32_t rdma_offset = 0;
      if( status->rdma_segments.empty() ) {
        NNTI_attrs_t nnti_attrs;
        transport->attrs(&nnti_attrs);
        lunasa::DataObject memory(nnti_attrs.mtu-nnti_attrs.max_eager_size, sizeof(completed_put_message_t), 
                                  lunasa::DataObject::AllocatorType::eager);
        completed_put_message_t *m = (completed_put_message_t *)memory.GetDataPtr();
        m->header.length = status->total_length;
        m->header.seed = status->seed;

        NNTI_work_request_t base_wr = NNTI_WR_INITIALIZER;
        NNTI_work_id_t      wid;

        std::queue<lunasa::DataObject::rdma_segment_desc> rdma_segments;

        memory.GetMetaRdmaHandles(rdma_segments);
        lunasa::DataObject::rdma_segment_desc rdma_segment = rdma_segments.front();
        rdma_buffer = (NNTI_buffer_t)rdma_segment.net_buffer_handle;
        rdma_offset = rdma_segment.net_buffer_offset;

        base_wr.op            = NNTI_OP_SEND;
        base_wr.flags         = (NNTI_op_flags_t)(NNTI_OF_LOCAL_EVENT | NNTI_OF_ZERO_COPY);
        base_wr.trans_hdl     = nnti::transports::transport::to_hdl(transport);
        base_wr.peer          = event->peer;
        base_wr.local_hdl     = rdma_buffer;
        base_wr.local_offset  = rdma_offset;
        base_wr.remote_hdl    = NNTI_INVALID_HANDLE;
        base_wr.remote_offset = 0;
        base_wr.length        = rdma_segment.size;

        nnti::datatype::nnti_event_callback send_callback(transport, send_callback_functor_root(send_promise));
        nnti::datatype::nnti_work_request wr(transport, base_wr, send_callback);

        transport->send(&wr, &wid);
        delete status;
        put_promise.set_value(1);
      } else {
        /* we currently assume that there's only one user data segment. */
        assert(status->rdma_segments.size() == 1); 

        lunasa::DataObject::rdma_segment_desc rdma_segment = status->rdma_segments.front();
        rdma_buffer = (NNTI_buffer_t)rdma_segment.net_buffer_handle;
        rdma_offset = rdma_segment.net_buffer_offset;
        status->rdma_segments.pop();

        NNTI_work_request_t base_wr = NNTI_WR_INITIALIZER;
        NNTI_work_id_t wid;

        base_wr.op            = NNTI_OP_PUT;
        base_wr.flags         = NNTI_OF_LOCAL_EVENT;
        base_wr.trans_hdl     = nnti::transports::transport::to_hdl(transport);
        base_wr.peer          = event->peer;
        base_wr.local_hdl     = rdma_buffer;
        /* This is a user data segment, the offset doesn't need to get us past the Lunasa headers */
        base_wr.local_offset  = rdma_offset;
        base_wr.remote_hdl    = status->remote_hdl;
        base_wr.remote_offset = status->remote_offset;
        base_wr.length        = rdma_segment.size;
        
        status->total_length += base_wr.length;
        status->remote_offset += base_wr.length;

        nnti::datatype::nnti_event_callback put_callback(transport, 
          put_callback_functor(put_promise, send_promise, transport, status));

        nnti::datatype::nnti_work_request wr(transport, base_wr, put_callback);
        transport->put(&wr, &wid);
      }
      return NNTI_EIO;
    }
};

class unexpected_callback_root
{
  private:
    nnti::transports::transport *transport = nullptr;
    std::promise<int>& recv_promise;
    std::promise<int>& put_promise; 
    std::promise<int>& send_promise; 
    NNTI_result_t recv_callback_func(NNTI_event_t *event, void *context)
    {
#ifdef DEBUG 
      std::cout << "This is a RECV callback function.  My parameters are event(" << (void*)event 
                << ") and context(" << (void*)context << ")" << std::endl;
#endif
        
      /* The RECEIVED message contains the REMOTE HANDLE for the RDMA buffer */
      NNTI_attrs_t nnti_attrs;
      transport->attrs(&nnti_attrs);
      
      lunasa::DataObject memory(nnti_attrs.mtu-nnti_attrs.max_eager_size, nnti_attrs.max_eager_size, 
                                lunasa::DataObject::AllocatorType::eager);
      NNTI_buffer_t rdma_buffer;
      uint32_t rdma_offset = 0;
      std::queue<lunasa::DataObject::rdma_segment_desc> rx_rdma_segments;
      memory.GetDataRdmaHandles(rx_rdma_segments);
      assert(rx_rdma_segments.size() == 1);

      lunasa::DataObject::rdma_segment_desc rdma_segment = rx_rdma_segments.front();
      rdma_buffer = (NNTI_buffer_t)rdma_segment.net_buffer_handle;
      rdma_offset = rdma_segment.net_buffer_offset;

      NNTI_event_t e;
      transport->next_unexpected(rdma_buffer, rdma_offset, &e);
      start_put_message_t *m = (start_put_message_t *)memory.GetDataPtr();

      NNTI_buffer_t remote_hdl;
      transport->dt_unpack((void *)&remote_hdl, &m->body[0], m->header.length);

      int payload_length = 128;
      unsigned int seed = std::time(0) & 0xffff;
      char *payload = (char *)malloc(payload_length);
      lunasa::DataObject put_source(payload, 0, payload_length, cleanup);
      std::srand(seed);
      for( int i = 0; i < payload_length; i++ ) {
        payload[i] = (char)(std::rand() & 0xff);
      }

      /* PUT the FIRST segment */
      std::queue<lunasa::DataObject::rdma_segment_desc> rdma_segments;
      put_source.GetHeaderRdmaHandles(rdma_segments);

      assert(rdma_segments.empty() == false);
      assert(rdma_segments.size() == 2);
      rdma_segment = rdma_segments.front();
      rdma_buffer = (NNTI_buffer_t)rdma_segment.net_buffer_handle;
      rdma_offset = rdma_segment.net_buffer_offset;
      rdma_segments.pop();

      NNTI_work_request_t base_wr = NNTI_WR_INITIALIZER;
      NNTI_work_id_t wid;

      base_wr.op            = NNTI_OP_PUT;
      base_wr.flags         = NNTI_OF_LOCAL_EVENT;
      base_wr.trans_hdl     = nnti::transports::transport::to_hdl(transport);
      base_wr.peer          = e.peer;
      base_wr.local_hdl     = rdma_buffer;
      /* This is a user data segment, the offset doesn't need to get us past the Lunasa headers */
      base_wr.local_offset  = rdma_offset; 
      base_wr.remote_hdl    = remote_hdl;
      base_wr.remote_offset = m->header.offset;
      base_wr.length        = rdma_segment.size;
      
      put_status *status = new put_status(base_wr.remote_offset+base_wr.length, rdma_segments, remote_hdl, 
                                          0, seed, put_source);

      nnti::datatype::nnti_event_callback put_callback(transport, 
        put_callback_functor(put_promise, send_promise, transport, status));

      nnti::datatype::nnti_work_request wr(transport, base_wr, put_callback);
      transport->put(&wr, &wid);

      recv_promise.set_value(1);
      return NNTI_EIO;
    }

  public:
      unexpected_callback_root(nnti::transports::transport *transport_, 
                               std::promise<int>& recv_promise_,
                               std::promise<int>& send_promise_,
                               std::promise<int>& put_promise_) : 
        transport(transport_),  
        recv_promise(recv_promise_), 
        send_promise(send_promise_), 
        put_promise(put_promise_)
      {}

      NNTI_result_t operator() (NNTI_event_t *event, void *context)
      {
          recv_callback_func(event, context);
          return NNTI_OK;
      }
};

class unexpected_callback_leaf
{
  private:
    nnti::transports::transport *transport = nullptr;
    std::promise< std::pair<uint32_t, uint32_t> >& recv_promise;

    NNTI_result_t recv_callback_func(NNTI_event_t *event, void *context)
    {
#ifdef DEBUG 
      std::cout << "This is a RECV callback function.  My parameters are event(" << (void*)event 
                << ") and context(" << (void*)context << ")" << std::endl;
#endif
        
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

      completed_put_message_t *m = (completed_put_message_t *)((char*)e.start + e.offset);
      int length = m->header.length;
      int seed = m->header.seed;

      recv_promise.set_value(std::pair<uint32_t, uint32_t>(length, seed));
      return NNTI_EIO;
    }

  public:
      unexpected_callback_leaf(nnti::transports::transport *transport_, 
                               std::promise< std::pair<uint32_t, uint32_t> >& recv_promise_) :
        transport(transport_),  
        recv_promise(recv_promise_)
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

  //There are two steps to initializing the components: Init and 
  //Start. The Init sets up basic configuration, but leaves the
  //component in a state where later components can update its 
  //settings. The Start option is used to officially start the
  //component. After this point the service cannot be modified.
  //If you don't need to do anything between Init and Start,
  //just use Start with a config and the bootstrap registration
  //function of the highest faodel service you need.
  faodel::bootstrap::Start(config, lunasa::bootstrap);

  /* 
   * We specified a default configuration (default_config) at compile time.  It is also 
   * possible to modify * the configuration at runtime.  The value of 
   * config.additional_files.env_name.if_defined specifies the name of an environment 
   * variable that will, in turn, contain the location of the runtime configuration.  
   * AppendFromReferences() updates our Configuration object based on the runtime 
   * configuration (if any).
   */
  config.AppendFromReferences();

  {
    send_future = send_promise.get_future();
    put_future = put_promise.get_future();

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
    config.Set("nnti.bind.address", nodeid.GetIP());
    config.Set("nnti.listen.port",  nodeid.GetPort());

    /* Initialize and configure our NNTI transport */
    transport = nnti::transports::factory::get_instance(config);
    transport->start();
    std::function< NNTI_result_t(NNTI_event_t*,void*) >  cb;

    /* 
     * Currently, this example only works for two MPI ranks.  So it's straightforward
     * to determine the rank of the other process.
     */
    partner_rank = mpi_rank ^ 0x1;

    if( mpi_rank < partner_rank ) {
      /* ==== ROOT process ==== */
      root_recv_future = root_recv_promise.get_future();
      cb = unexpected_callback_root(transport, root_recv_promise, send_promise, put_promise);
    } else {
      /* ==== LEAF process ==== */
      leaf_recv_future = leaf_recv_promise.get_future();
      cb = unexpected_callback_leaf(transport, leaf_recv_promise);
    }
    nnti::datatype::nnti_event_callback recv_cb(transport, cb);

    NNTI_result_t result = transport->eq_create(128, NNTI_EQF_UNEXPECTED, recv_cb, nullptr, &unexpected_eq_);
    assert(result == NNTI_OK);

    /* Tell Lunasa how to register and de-register memory with NNTI */
    lunasa::RegisterPinUnpin(register_memory_func(transport), unregister_memory_func(transport));

    /* Exchange NodeIDs with our partner. */
    faodel::nodeid_t partner_nodeid; 
    MPI_Request request;
    rc = MPI_Isend((void *)&nodeid, sizeof(faodel::nodeid_t), MPI_CHAR, partner_rank, 0, MPI_COMM_WORLD, &request);
    assert(rc == MPI_SUCCESS);

    MPI_Status status;
    rc = MPI_Recv((void *)&partner_nodeid, sizeof(faodel::nodeid_t), MPI_CHAR, partner_rank, 0, MPI_COMM_WORLD, &status);
    assert(rc == MPI_SUCCESS);

    if( mpi_rank < partner_rank ) {
      /* ==== ROOT process : the source of the RDMA put ==== */
      root_recv_future.get();
      put_future.get();
      send_future.get();
    } else {
      /* ==== LEAF process : the destination of the RDMA put ==== */
      NNTI_peer_t p;
      transport->connect(partner_nodeid.GetHttpLink().c_str(), 1000, &p);

      int payload_length = 8;
      NNTI_attrs_t nnti_attrs;
      transport->attrs(&nnti_attrs);

      /* Get buffer to send RDMA information */
      lunasa::DataObject foo(0, 128, lunasa::DataObject::AllocatorType::eager);
      lunasa::DataObject put_target(0, 128, lunasa::DataObject::AllocatorType::eager);
      char *payload = (char *)put_target.GetDataPtr();
      char *put_tgt_payload = (char *)put_target.GetMetaPtr();
      memset(payload, 0xFF, 128);

      NNTI_buffer_t rdma_put_buffer;
      uint32_t rdma_put_offset = 0;
      std::queue<lunasa::DataObject::rdma_segment_desc> rdma_put_segments;
      put_target.GetHeaderRdmaHandles(rdma_put_segments);
      assert(rdma_put_segments.size() == 1);

      lunasa::DataObject::rdma_segment_desc rdma_put_segment = rdma_put_segments.front();
      rdma_put_buffer = (NNTI_buffer_t)rdma_put_segment.net_buffer_handle;
      rdma_put_offset = rdma_put_segment.net_buffer_offset;

      lunasa::DataObject memory(nnti_attrs.mtu-nnti_attrs.max_eager_size, 128, 
                                lunasa::DataObject::AllocatorType::eager);
      NNTI_buffer_t rdma_buffer;
      uint32_t rdma_offset = 0;
      NNTI_buffer_t hdr_rdma_buffer;
      uint32_t hdr_rdma_offset = 0;
      std::queue<lunasa::DataObject::rdma_segment_desc> rdma_segments;

      memory.GetMetaRdmaHandles(rdma_segments);
      assert(rdma_segments.size() == 1);

      lunasa::DataObject::rdma_segment_desc rdma_segment = rdma_segments.front();
      rdma_buffer = (NNTI_buffer_t)rdma_segment.net_buffer_handle;
      rdma_offset = rdma_segment.net_buffer_offset;
      std::queue<lunasa::DataObject::rdma_segment_desc> hdr_rdma_segments;
      memory.GetHeaderRdmaHandles(hdr_rdma_segments);
      assert(hdr_rdma_segments.size() == 1);

      lunasa::DataObject::rdma_segment_desc hdr_rdma_segment = hdr_rdma_segments.front();
      hdr_rdma_buffer = (NNTI_buffer_t)hdr_rdma_segment.net_buffer_handle;
      hdr_rdma_offset = hdr_rdma_segment.net_buffer_offset;

      char data[MAX_NET_BUFFER_REMOTE_SIZE];
      memset((void *)data, 0xFF, MAX_NET_BUFFER_REMOTE_SIZE);

      start_put_message_t *m = (start_put_message_t *)memory.GetDataPtr();
      // !!!! TODO: Leaf process should advertise size of remote LDO
      m->header.offset = rdma_put_offset;
      m->header.length = sizeof(start_put_message_t) - offsetof(start_put_message_t, body);
      transport->dt_pack((void *)rdma_put_buffer, m->body, MAX_NET_BUFFER_REMOTE_SIZE-8);
      
      NNTI_work_request_t base_wr = NNTI_WR_INITIALIZER;
      NNTI_work_id_t      wid;

      base_wr.op            = NNTI_OP_SEND;
      base_wr.flags         = (NNTI_op_flags_t)(NNTI_OF_LOCAL_EVENT | NNTI_OF_ZERO_COPY);
      base_wr.trans_hdl     = nnti::transports::transport::to_hdl(transport);
      base_wr.peer          = p;
      base_wr.local_hdl     = rdma_buffer;
      base_wr.local_offset  = rdma_offset;
      base_wr.remote_hdl    = NNTI_INVALID_HANDLE;
      base_wr.remote_offset = 0;
      base_wr.length        = memory.GetMetaSize() + memory.GetDataSize();

      nnti::datatype::nnti_event_callback send_callback(transport, send_callback_functor_leaf(send_promise));
      nnti::datatype::nnti_work_request wr(transport, base_wr, send_callback);

      transport->send(&wr, &wid);

      send_future.get();
      std::pair<uint32_t, uint32_t> put_details = leaf_recv_future.get();

      uint32_t length = put_details.first;
      assert(length == put_target.GetDataSize());
      uint32_t seed = put_details.second;
      char *msg = (char *)put_target.GetDataPtr();
      std::srand(seed);
      for( int i = 0; i < put_target.GetDataSize(); i++ ) {
          char expected = (std::rand() & 0xff);
          char received = (*msg++ & 0xff);
          assert(received == expected);
      }
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
