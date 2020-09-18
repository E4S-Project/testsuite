// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 


#ifndef TEST_UTILS_HPP
#define TEST_UTILS_HPP

//#include "nnti/nnti_pch.hpp"

#include "nnti/nntiConfig.h"

#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <zlib.h>

#include <assert.h>

#include <iostream>
#include <sstream>
#include <thread>

#include <chrono>

#include "nnti/nnti_logger.hpp"

#include "nnti/nnti_util.hpp"

#include "nnti/nnti_transport.hpp"
#include "nnti/nnti_buffer.hpp"
#include "nnti/nnti_wid.hpp"


struct buffer_properties {
    NNTI_buffer_t  hdl;
    char          *base;
    uint64_t       size;
    uint64_t       offset;
};

class callback {
public:
    NNTI_result_t operator() (NNTI_event_t *event, void *context) {
        std::cout << "This is a callback object.  My parameters are event(" << (void*)event << ") and context(" << (void*)context << ")" << std::endl;
        return NNTI_EIO;
    }
};

NNTI_result_t
find_server_urls(int num_servers,
                 char *my_url,
                 char server_url[][NNTI_URL_LEN],
                 bool &i_am_server);

NNTI_result_t
cb_func(NNTI_event_t *event, void *context);

NNTI_result_t
send_target_hdl(nnti::transports::transport *t,
                NNTI_buffer_t                send_hdl,
                char                        *send_base,
                uint64_t                     send_size,
                NNTI_buffer_t                target_hdl,
                NNTI_peer_t                  peer_hdl,
                NNTI_event_queue_t           eq);

NNTI_result_t
recv_target_hdl(nnti::transports::transport *t,
                NNTI_buffer_t                recv_hdl,
                char                        *recv_base,
                NNTI_buffer_t               *target_hdl,
                NNTI_peer_t                 *peer_hdl,
                NNTI_event_queue_t           eq);

NNTI_result_t
send_hdl(nnti::transports::transport *t,
         NNTI_buffer_t                hdl,
         char                        *hdl_base,
         uint32_t                     hdl_size,
         NNTI_peer_t                  peer_hdl,
         NNTI_event_queue_t           eq);

NNTI_result_t
recv_hdl(nnti::transports::transport *t,
         NNTI_buffer_t                recv_hdl,
         char                        *recv_base,
         uint32_t                     recv_size,
         NNTI_buffer_t               *hdl,
         NNTI_peer_t                 *peer_hdl,
         NNTI_event_queue_t           eq);

NNTI_result_t
send_ack(nnti::transports::transport *t,
         NNTI_buffer_t                hdl,
         NNTI_buffer_t                ack_hdl,
         NNTI_peer_t                  peer_hdl,
         NNTI_event_queue_t           eq);

NNTI_result_t
recv_ack(nnti::transports::transport *t,
         NNTI_buffer_t                ack_hdl,
         NNTI_peer_t                 *peer_hdl,
         NNTI_event_queue_t           eq);

NNTI_result_t
populate_buffer(nnti::transports::transport *t,
                uint32_t                     seed,
                uint64_t                     offset_multiplier,
                NNTI_buffer_t                buf_hdl,
                char                        *buf_base,
                uint64_t                     buf_size);

bool
verify_buffer(char     *buf_base,
              uint64_t  buf_offset,
              uint64_t  buf_size);

NNTI_result_t
wait_data(nnti::transports::transport *t,
          NNTI_event_queue_t           eq);

NNTI_result_t
send_data_async(nnti::transports::transport          *t,
                uint64_t                             offset_multiplier,
                NNTI_buffer_t                        src_hdl,
                NNTI_buffer_t                        dst_hdl,
                NNTI_peer_t                          peer_hdl,
                nnti::datatype::nnti_event_callback &cb,
                void                                *context);

NNTI_result_t
send_data_async(nnti::transports::transport *t,
                uint64_t                     offset_multiplier,
                NNTI_buffer_t                src_hdl,
                NNTI_buffer_t                dst_hdl,
                NNTI_peer_t                  peer_hdl);

NNTI_result_t
send_data(nnti::transports::transport          *t,
          uint64_t                             offset_multiplier,
          NNTI_buffer_t                        src_hdl,
          NNTI_buffer_t                        dst_hdl,
          NNTI_peer_t                          peer_hdl,
          NNTI_event_queue_t                   eq,
          nnti::datatype::nnti_event_callback &cb,
          void                                *context);

NNTI_result_t
send_data(nnti::transports::transport *t,
          uint64_t                     offset_multiplier,
          NNTI_buffer_t                src_hdl,
          NNTI_buffer_t                dst_hdl,
          NNTI_peer_t                  peer_hdl,
          NNTI_event_queue_t           eq);

NNTI_result_t
recv_data(nnti::transports::transport *t,
          NNTI_event_queue_t           eq,
          NNTI_event_t                *event);

NNTI_result_t
get_data_async(nnti::transports::transport         *t,
               NNTI_buffer_t                        src_hdl,
               NNTI_buffer_t                        dst_hdl,
               NNTI_peer_t                          peer_hdl,
               nnti::datatype::nnti_event_callback &cb,
               void                                *context);

NNTI_result_t
get_data_async(nnti::transports::transport *t,
               NNTI_buffer_t                src_hdl,
               NNTI_buffer_t                dst_hdl,
               NNTI_peer_t                  peer_hdl);

NNTI_result_t
get_data(nnti::transports::transport         *t,
         NNTI_buffer_t                        src_hdl,
         NNTI_buffer_t                        dst_hdl,
         NNTI_peer_t                          peer_hdl,
         NNTI_event_queue_t                   eq,
         nnti::datatype::nnti_event_callback &cb,
         void                                *context);

NNTI_result_t
get_data(nnti::transports::transport *t,
         NNTI_buffer_t                src_hdl,
         NNTI_buffer_t                dst_hdl,
         NNTI_peer_t                  peer_hdl,
         NNTI_event_queue_t           eq);

NNTI_result_t
put_data_async(nnti::transports::transport         *t,
               NNTI_buffer_t                        src_hdl,
               NNTI_buffer_t                        dst_hdl,
               NNTI_peer_t                          peer_hdl,
               nnti::datatype::nnti_event_callback &cb,
               void                                *context);

NNTI_result_t
put_data_async(nnti::transports::transport *t,
               NNTI_buffer_t                src_hdl,
               NNTI_buffer_t                dst_hdl,
               NNTI_peer_t                  peer_hdl);

NNTI_result_t
put_data(nnti::transports::transport         *t,
         NNTI_buffer_t                        src_hdl,
         NNTI_buffer_t                        dst_hdl,
         NNTI_peer_t                          peer_hdl,
         NNTI_event_queue_t                   eq,
         nnti::datatype::nnti_event_callback &cb,
         void                                *context);

NNTI_result_t
put_data(nnti::transports::transport *t,
         NNTI_buffer_t                src_hdl,
         NNTI_buffer_t                dst_hdl,
         NNTI_peer_t                  peer_hdl,
         NNTI_event_queue_t           eq);

#endif /* TEST_UTILS_HPP */
